#include "PhysicsEngine.h"
#include "CollisionDetectionSAT.h"
#include "NCLDebug.h"
#include "Object.h"
#include <nclgl\Window.h>
#include <omp.h>

void PhysicsEngine::SetDefaults()
{
  m_DebugDrawFlags = NULL;
  m_IsPaused = false;
  m_UpdateTimestep = 1.0f / 60.f;
  m_UpdateAccum = 0.0f;
  m_Gravity = Vector3(0.0f, -9.81f, 0.0f);
  m_DampingFactor = 0.999f;
  m_integrationType = INTEGRATION_SEMI_IMPLICIT_EULER;
}

PhysicsEngine::PhysicsEngine()
{
  SetDefaults();
}

PhysicsEngine::~PhysicsEngine()
{
  RemoveAllPhysicsObjects();
}

void PhysicsEngine::AddPhysicsObject(PhysicsObject *obj)
{
  m_PhysicsObjects.push_back(obj);
}

void PhysicsEngine::RemovePhysicsObject(PhysicsObject *obj)
{
  // Lookup the object in question
  auto found_loc = std::find(m_PhysicsObjects.begin(), m_PhysicsObjects.end(), obj);

  // If found, remove it from the list
  if (found_loc != m_PhysicsObjects.end())
  {
    m_PhysicsObjects.erase(found_loc);
  }
}

void PhysicsEngine::RemoveAllPhysicsObjects()
{
  // Delete and remove all constraints/collision manifolds
  for (Constraint *c : m_vpConstraints)
  {
    delete c;
  }
  m_vpConstraints.clear();

  for (Manifold *m : m_vpManifolds)
  {
    delete m;
  }
  m_vpManifolds.clear();

  // Delete and remove all physics objects
  // - we also need to inform the (possible) associated game-object
  //   that the physics object no longer exists
  for (PhysicsObject *obj : m_PhysicsObjects)
  {
    if (obj->m_pParent != NULL)
      obj->m_pParent->m_pPhysicsObject = NULL;
    delete obj;
  }
  m_PhysicsObjects.clear();
}

void PhysicsEngine::Update(float deltaTime)
{
  const int max_updates_per_frame = 5;

  if (!m_IsPaused)
  {
    m_UpdateAccum += deltaTime;
    for (int i = 0; (m_UpdateAccum >= m_UpdateTimestep) && i < max_updates_per_frame; ++i)
    {
      m_UpdateAccum -= m_UpdateTimestep;
      if (!m_IsPaused)
        UpdatePhysics(); // Additional check here incase physics was paused
                         // mid-update and the contents of the physics need to
                         // be displayed
    }

    if (m_UpdateAccum >= m_UpdateTimestep)
    {
      NCLERROR("Physics too slow to run in real time!");
      // Drop Time in the hope that it can continue to run in real-time
      m_UpdateAccum = 0.0f;
    }
  }
}

void PhysicsEngine::UpdatePhysics()
{
  for (Manifold *m : m_vpManifolds)
  {
    delete m;
  }
  m_vpManifolds.clear();

  // Check for collisions
  BroadPhaseCollisions();
  NarrowPhaseCollisions();

  // Solve collision constraints
  SolveConstraints();

  // Update movement
  for (PhysicsObject *obj : m_PhysicsObjects)
  {
    UpdatePhysicsObject(obj);
  }
}

void PhysicsEngine::SolveConstraints()
{
  // Optional step to allow constraints to
  // precompute values based off current velocities
  // before they are updated in the main loop below.
  for (Manifold *m : m_vpManifolds)
    m->PreSolverStep(m_UpdateTimestep);

  for (Constraint *c : m_vpConstraints)
    c->PreSolverStep(m_UpdateTimestep);

  for (size_t i = 0; i < SOLVER_ITERATIONS; i++)
  {
    // Solve all Constraints and Collision Manifolds
    for (Manifold *m : m_vpManifolds)
      m->ApplyImpulse();

    for (Constraint *c : m_vpConstraints)
      c->ApplyImpulse();
  }
}

void PhysicsEngine::UpdatePhysicsObject(PhysicsObject *obj)
{
  // Apply gravity
  if (obj->m_InvMass > 0.0f)
    obj->m_LinearVelocity += m_Gravity * m_UpdateTimestep;

  switch (m_integrationType)
  {
  case INTEGRATION_EXPLICIT_EULER:
  {
    // Update position
    obj->m_Position += obj->m_LinearVelocity * m_UpdateTimestep;

    // Update linear velocity (v = u + at)
    obj->m_LinearVelocity += obj->m_Force * obj->m_InvMass * m_UpdateTimestep;

    // Linear velocity damping
    obj->m_LinearVelocity = obj->m_LinearVelocity * m_DampingFactor;

    // Update orientation
    obj->m_Orientation = obj->m_Orientation + (obj->m_Orientation * (obj->m_AngularVelocity * m_UpdateTimestep * 0.5f));
    obj->m_Orientation.Normalise();

    // Update angular velocity
    obj->m_AngularVelocity += obj->m_InvInertia * obj->m_Torque * m_UpdateTimestep;

    // Angular velocity damping
    obj->m_AngularVelocity = obj->m_AngularVelocity * m_DampingFactor;

    break;
  }

  default:
  case INTEGRATION_SEMI_IMPLICIT_EULER:
  {
    // Update linear velocity (v = u + at)
    obj->m_LinearVelocity += obj->m_Force * obj->m_InvMass * m_UpdateTimestep;

    // Linear velocity damping
    obj->m_LinearVelocity = obj->m_LinearVelocity * m_DampingFactor;

    // Update position
    obj->m_Position += obj->m_LinearVelocity * m_UpdateTimestep;

    // Update angular velocity
    obj->m_AngularVelocity += obj->m_InvInertia * obj->m_Torque * m_UpdateTimestep;

    // Angular velocity damping
    obj->m_AngularVelocity = obj->m_AngularVelocity * m_DampingFactor;

    // Update orientation
    obj->m_Orientation = obj->m_Orientation + (obj->m_Orientation * (obj->m_AngularVelocity * m_UpdateTimestep * 0.5f));
    obj->m_Orientation.Normalise();

    break;
  }

  case INTEGRATION_RUNGE_KUTTA_2:
  {
    // TODO
    break;
  }

  case INTEGRATION_RUNGE_KUTTA_4:
  {
    // TODO
    break;
  }
  }

  // Mark cached world transform as invalid
  obj->m_wsTransformInvalidated = true;
}

void PhysicsEngine::BroadPhaseCollisions()
{
  m_BroadphaseCollisionPairs.clear();

  PhysicsObject *m_pObj1, *m_pObj2;
  //	The broadphase needs to build a list of all potentially colliding objects
  // in the world,
  //	which then get accurately assesed in narrowphase. If this is too coarse
  // then the system slows down with
  //	the complexity of narrowphase collision checking, if this is too fine then
  // collisions may be missed.

  //	Brute force approach.
  //  - For every object A, assume it could collide with every other object..
  //    even if they are on the opposite sides of the world.
  if (m_PhysicsObjects.size() > 0)
  {
    for (size_t i = 0; i < m_PhysicsObjects.size() - 1; ++i)
    {
      for (size_t j = i + 1; j < m_PhysicsObjects.size(); ++j)
      {
        m_pObj1 = m_PhysicsObjects[i];
        m_pObj2 = m_PhysicsObjects[j];

        // Check they both atleast have collision shapes
        if (m_pObj1->NumCollisionShapes() > 0 && m_pObj2->NumCollisionShapes() > 0)
        {
          CollisionPair cp;
          cp.pObjectA = m_pObj1;
          cp.pObjectB = m_pObj2;
          m_BroadphaseCollisionPairs.push_back(cp);
        }
      }
    }
  }
}

void PhysicsEngine::NarrowPhaseCollisions()
{
  if (m_BroadphaseCollisionPairs.size() > 0)
  {
    // Collision data to pass between detection and manifold generation stages.
    CollisionData colData;

    // Collision Detection Algorithm to use
    CollisionDetectionSAT colDetect;

    // Iterate over all possible collision pairs and perform accurate collision
    // detection
    for (size_t i = 0; i < m_BroadphaseCollisionPairs.size(); ++i)
    {
      CollisionPair &cp = m_BroadphaseCollisionPairs[i];

      for (auto aIt = cp.pObjectA->CollisionShapesBegin(); aIt != cp.pObjectA->CollisionShapesEnd(); ++aIt)
      {
        for (auto bIt = cp.pObjectB->CollisionShapesBegin(); bIt != cp.pObjectB->CollisionShapesEnd(); ++bIt)
        {
          CollisionShape *shapeA = *aIt;
          CollisionShape *shapeB = *bIt;

      colDetect.BeginNewPair(cp.pObjectA, cp.pObjectB, shapeA, shapeB);

      // Detects if the objects are colliding - Seperating Axis Theorem
      if (colDetect.AreColliding(&colData))
      {
        // Draw collision data to the window if requested
        // - Have to do this here as colData is only temporary.
        if (m_DebugDrawFlags & DEBUGDRAW_FLAGS_COLLISIONNORMALS)
        {
          NCLDebug::DrawPointNDT(colData._pointOnPlane, 0.1f, Vector4(0.5f, 0.5f, 1.0f, 1.0f));
          NCLDebug::DrawThickLineNDT(colData._pointOnPlane,
                                     colData._pointOnPlane - colData._normal * colData._penetration, 0.05f,
                                     Vector4(0.0f, 0.0f, 1.0f, 1.0f));
        }

        // Check to see if any of the objects have collision callbacks that dont
        // want the objects to physically collide
        bool okA = cp.pObjectA->FireOnCollisionEvent(cp.pObjectA, cp.pObjectB);
        bool okB = cp.pObjectB->FireOnCollisionEvent(cp.pObjectB, cp.pObjectA);

        if (okA && okB)
        {
          //-- TUTORIAL 5 CODE --
          // Build full collision manifold that will also handle the collision
          // response between the two objects in the solver stage
          Manifold *manifold = new Manifold();
          manifold->Initiate(cp.pObjectA, cp.pObjectB);

          // Construct contact points that form the perimeter of the collision
          // manifold
          colDetect.GenContactPoints(manifold);

          // Add to list of manifolds that need solving
          m_vpManifolds.push_back(manifold);
        }
      }
    }
  }
    }
  }
}

void PhysicsEngine::DebugRender()
{
  // Draw all collision manifolds
  if (m_DebugDrawFlags & DEBUGDRAW_FLAGS_MANIFOLD)
  {
    for (Manifold *m : m_vpManifolds)
    {
      m->DebugDraw();
    }
  }

  // Draw all constraints
  if (m_DebugDrawFlags & DEBUGDRAW_FLAGS_CONSTRAINT)
  {
    for (Constraint *c : m_vpConstraints)
    {
      c->DebugDraw();
    }
  }

  // Draw all associated collision shapes
  if (m_DebugDrawFlags & DEBUGDRAW_FLAGS_COLLISIONVOLUMES)
  {
    for (PhysicsObject *obj : m_PhysicsObjects)
    {
      for (auto it = obj->CollisionShapesBegin(); it != obj->CollisionShapesEnd(); ++it)
        (*it)->DebugDraw(obj);
    }
  }
}