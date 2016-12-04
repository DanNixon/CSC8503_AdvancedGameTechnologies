#include "PhysicsEngine.h"
#include "CollisionDetectionSAT.h"
#include "NCLDebug.h"
#include "Object.h"
#include <algorithm>
#include <nclgl\Window.h>
#include <omp.h>

void PhysicsEngine::SetDefaults()
{
  m_DebugDrawFlags = NULL;
  m_IsPaused = false;
  m_UpdateTimestep = 1.0f / 60.f;
  m_UpdateAccum = 0.0f;
  m_LinearGravity = Vector3(0.0f, -9.81f, 0.0f);
  m_PointGravity = -9.81f;
  m_PointGravitation = 6.674e-11f;
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
    delete c;
  m_vpConstraints.clear();

  for (Manifold *m : m_vpManifolds)
    delete m;
  m_vpManifolds.clear();

  // Delete and remove all physics objects
  // - we also need to inform the (possible) associated game-object
  //   that the physics object no longer exists
  for (PhysicsObject *obj : m_PhysicsObjects)
  {
    if (obj->m_parent != NULL)
      obj->m_parent->m_pPhysicsObject = NULL;
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
    delete m;
  m_vpManifolds.clear();

  // Check for collisions
  BroadPhaseCollisions();
  NarrowPhaseCollisions();

  // Solve collision constraints
  SolveConstraints();

  // Update movement
  for (PhysicsObject *obj : m_PhysicsObjects)
    UpdatePhysicsObject(obj);
}

void PhysicsEngine::SolveConstraints()
{
  // Optional step to allow constraints to precompute values based off current velocities before they are updated in the main loop below.
  for (Manifold *m : m_vpManifolds)
    m->PreSolverStep(m_UpdateTimestep);

  for (Constraint *c : m_vpConstraints)
    c->PreSolverStep(m_UpdateTimestep);

  // Solve all Constraints and Collision Manifolds
  for (size_t i = 0; i < SOLVER_ITERATIONS; i++)
  {
    for (Manifold *m : m_vpManifolds)
      m->ApplyImpulse();

    for (Constraint *c : m_vpConstraints)
      c->ApplyImpulse();
  }
}

void PhysicsEngine::UpdatePhysicsObject(PhysicsObject *obj)
{
  // Skip if object is at rest
  if (obj->IsAtRest())
    return;

  // Apply gravity
  if (obj->m_InvMass > 0.0f)
  {
    if (obj->m_gravitationTarget == nullptr)
    {
      // Uniform directional gravity
      obj->m_LinearVelocity += m_LinearGravity * m_UpdateTimestep;
    }
    else
    {
      Vector3 ab = obj->m_gravitationTarget->m_Position - obj->m_Position;
      Vector3 abn = ab;
      abn.Normalise();

      if (obj->m_gravitationTarget->m_InvMass > 0.0f)
      {
        // Handle gravity between points (target is pulled towards self)
        float r2 = ab.LengthSquared();
        float f = m_PointGravitation / (r2 * obj->m_InvMass * obj->m_gravitationTarget->m_InvMass);
        obj->ApplyForce(abn * f);
        obj->m_gravitationTarget->ApplyForce(abn * -f);
        obj->m_gravitationTarget->WakeUp();
      }
      else
      {
        // Handle gravity between points (target is immovable)
        obj->m_LinearVelocity += abn * -m_PointGravity * m_UpdateTimestep;
      }
    }
  }

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

  // Test for rest conditions
  obj->DoAtRestTest();

  // Mark cached world transform as invalid
  obj->m_wsTransformInvalidated = true;

  // TODO
  obj->ClearForces();
}

void PhysicsEngine::BroadPhaseCollisions()
{
  m_BroadphaseCollisionPairs.clear();

  PhysicsObject *obj1, *obj2;
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
        obj1 = m_PhysicsObjects[i];
        obj2 = m_PhysicsObjects[j];

        // Check they both atleast have collision shapes
        if (obj1->NumCollisionShapes() > 0 && obj2->NumCollisionShapes() > 0 && (obj1->IsAwake() || obj2->IsAwake()))
        {
          CollisionPair cp;
          cp.pObjectA = obj1;
          cp.pObjectB = obj2;
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

/**
 * @brief Checks if all physics objects in the system have the at rest flags set.
 * @return True if all objects are flagged as at rest
 *
 * Mainly added for debugging.
 */
bool PhysicsEngine::SimulationIsAtRest() const
{
  auto firstObjectNotAtRest =
      std::find_if(m_PhysicsObjects.begin(), m_PhysicsObjects.end(), [](PhysicsObject *o) { return o->IsAwake(); });
  return (firstObjectNotAtRest == m_PhysicsObjects.end());
}