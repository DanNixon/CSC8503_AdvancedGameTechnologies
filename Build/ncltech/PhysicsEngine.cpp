#include "PhysicsEngine.h"

#include "CollisionDetectionSAT.h"
#include "IntegrationHelpers.h"
#include "NCLDebug.h"
#include "Object.h"
#include <algorithm>
#include <nclgl\Window.h>
#include <omp.h>

/**
 * @brief Sets default engine settings.
 */
void PhysicsEngine::SetDefaults()
{
  m_DebugDrawFlags = NULL;
  m_IsPaused = false;
  m_UpdateTimestep = 1.0f / 60.f;
  m_UpdateAccum = 0.0f;
  m_LinearGravity = Vector3(0.0f, -9.81f, 0.0f);
  m_PointGravity = -9.81f;
  m_PointGravitation = 6.674e-11f;
  m_integrationType = INTEGRATION_SEMI_IMPLICIT_EULER;
}

/**
 * @brief Creates a new physics engine.
 *
 * (handled by singleton)
 */
PhysicsEngine::PhysicsEngine()
    : m_broadphaseDetection(nullptr)
{
  SetDefaults();
}

PhysicsEngine::~PhysicsEngine()
{
  RemoveAllPhysicsObjects();

  if (m_broadphaseDetection != nullptr)
    delete m_broadphaseDetection;
}

/**
 * @brief Adds a new object to the simulation.
 * @param obj Object to add
 */
void PhysicsEngine::AddPhysicsObject(PhysicsObject *obj)
{
  m_PhysicsObjects.push_back(obj);
}

/**
 * @brief Removes an object from the simulation.
 * @param obj Object to remove
 */
void PhysicsEngine::RemovePhysicsObject(PhysicsObject *obj)
{
  // Lookup the object in question
  auto it = std::find(m_PhysicsObjects.begin(), m_PhysicsObjects.end(), obj);

  // If found, remove it from the list
  if (it != m_PhysicsObjects.end())
    m_PhysicsObjects.erase(it);
}

/**
 * @brief Removes all physics objects from the simulation.
 */
void PhysicsEngine::RemoveAllPhysicsObjects()
{
  // Delete and remove all constraints/collision manifolds
  for (IConstraint *c : m_vpConstraints)
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

/**
 * @brief Performs an update of the physics system.
 * @param deltaTime Time passed in seconds
 */
void PhysicsEngine::Update(float deltaTime)
{
  m_broadphaseCollisionPairCount = 0;

  if (!m_IsPaused)
  {
    m_UpdateAccum += deltaTime;

    for (uint8_t i = 0; (m_UpdateAccum >= m_UpdateTimestep) && i < MAX_UPDATES_PER_FRAME; ++i)
    {
      m_UpdateAccum -= m_UpdateTimestep;

      // Additional check here in case physics was paused mid-update and the contents of the physics need to be displayed
      if (!m_IsPaused)
      {
        UpdatePhysics();
        m_broadphaseCollisionPairCount += m_BroadphaseCollisionPairs.size();
      }
    }

    if (m_UpdateAccum >= m_UpdateTimestep)
    {
      NCLERROR("Physics too slow to run in real time!");
      // Drop Time in the hope that it can continue to run in real-time
      m_UpdateAccum = 0.0f;
    }
  }
}

/**
 * @brief Updates the physical objects in the simulation.
 */
void PhysicsEngine::UpdatePhysics()
{
  for (Manifold *m : m_vpManifolds)
    delete m;
  m_vpManifolds.clear();

  // Broadphase collision detection
  m_BroadphaseCollisionPairs.clear();
  m_broadphaseDetection->FindPotentialCollisionPairs(m_PhysicsObjects, m_BroadphaseCollisionPairs);
  if (m_DebugDrawFlags & DEBUGDRAW_FLAGS_BROADPHASE)
    m_broadphaseDetection->DebugDraw();

  // Narrowphase collision detection
  NarrowPhaseCollisions();

  // Solve collision constraints
  SolveConstraints();

  // Update movement
  for (PhysicsObject *obj : m_PhysicsObjects)
    UpdatePhysicsObject(obj);
}

/**
 * @brief Solves constraints between objects.
 */
void PhysicsEngine::SolveConstraints()
{
  // Optional step to allow constraints to precompute values based off current velocities before they are updated in the
  // main loop below.
  for (Manifold *m : m_vpManifolds)
    m->PreSolverStep(m_UpdateTimestep);

  for (IConstraint *c : m_vpConstraints)
    c->PreSolverStep(m_UpdateTimestep);

  // Solve all Constraints and Collision Manifolds
  for (size_t i = 0; i < SOLVER_ITERATIONS; i++)
  {
    for (Manifold *m : m_vpManifolds)
      m->ApplyImpulse();

    for (IConstraint *c : m_vpConstraints)
      c->ApplyImpulse();
  }
}

/**
 * @brief Updates the position and velocity of a given object based on their state and world gravity.
 * @param obj Object to update
 */
void PhysicsEngine::UpdatePhysicsObject(PhysicsObject *obj)
{
  if (obj->IsAwake())
  {
    float damping = obj->m_dampingCoefficient;

    // Apply gravity
    if (obj->m_inverseMass > 0.0f)
    {
      if (obj->m_gravitationTarget == nullptr)
      {
        // Uniform directional gravity
        obj->m_linearVelocity += m_LinearGravity * m_UpdateTimestep;
      }
      else
      {
        Vector3 ab = obj->m_gravitationTarget->m_position - obj->m_position;
        Vector3 abn = ab;
        abn.Normalise();

        if (obj->m_gravitationTarget->m_inverseMass > 0.0f)
        {
          // Handle gravity between points (target is pulled towards self)
          float r2 = ab.LengthSquared();
          float f = m_PointGravitation / (r2 * obj->m_inverseMass * obj->m_gravitationTarget->m_inverseMass);
          obj->ApplyForce(abn * f);
          obj->m_gravitationTarget->ApplyForce(abn * -f);
          obj->m_gravitationTarget->WakeUp();
        }
        else
        {
          // Handle gravity between points (target is immovable)
          obj->m_linearVelocity += abn * -m_PointGravity * m_UpdateTimestep;
        }
      }
    }

    switch (m_integrationType)
    {
    case INTEGRATION_EXPLICIT_EULER:
    {
      // Update position
      obj->m_position += obj->m_linearVelocity * m_UpdateTimestep;

      // Update linear velocity (v = u + at)
      obj->m_linearVelocity += obj->m_linearForce * obj->m_inverseMass * m_UpdateTimestep;

      // Linear velocity damping
      obj->m_linearVelocity = obj->m_linearVelocity * damping;

      // Update orientation
      obj->m_orientation = obj->m_orientation + (obj->m_orientation * (obj->m_angularVelocity * m_UpdateTimestep * 0.5f));
      obj->m_orientation.Normalise();

      // Update angular velocity
      obj->m_angularVelocity += obj->m_inverseInertia * obj->m_torque * m_UpdateTimestep;

      // Angular velocity damping
      obj->m_angularVelocity = obj->m_angularVelocity * damping;

      break;
    }

    default:
    case INTEGRATION_SEMI_IMPLICIT_EULER:
    {
      // Update linear velocity (v = u + at)
      obj->m_linearVelocity += obj->m_linearForce * obj->m_inverseMass * m_UpdateTimestep;

      // Linear velocity damping
      obj->m_linearVelocity = obj->m_linearVelocity * damping;

      // Update position
      obj->m_position += obj->m_linearVelocity * m_UpdateTimestep;

      // Update angular velocity
      obj->m_angularVelocity += obj->m_inverseInertia * obj->m_torque * m_UpdateTimestep;

      // Angular velocity damping
      obj->m_angularVelocity = obj->m_angularVelocity * damping;

      // Update orientation
      obj->m_orientation = obj->m_orientation + (obj->m_orientation * (obj->m_angularVelocity * m_UpdateTimestep * 0.5f));
      obj->m_orientation.Normalise();

      break;
    }

    case INTEGRATION_RUNGE_KUTTA_2:
    {
      // RK2 integration for linear motion
      IntegrationHelpers::State state = {obj->m_position, obj->m_linearVelocity, obj->m_linearForce * obj->m_inverseMass};
      IntegrationHelpers::RK2(state, m_UpdateTimestep);
      obj->m_position = state.position;
      obj->m_linearVelocity = state.velocity;

      // Linear velocity damping
      obj->m_linearVelocity = obj->m_linearVelocity * damping;

      // Update angular velocity
      obj->m_angularVelocity += obj->m_inverseInertia * obj->m_torque * m_UpdateTimestep;

      // Angular velocity damping
      obj->m_angularVelocity = obj->m_angularVelocity * damping;

      // Update orientation
      obj->m_orientation = obj->m_orientation + (obj->m_orientation * (obj->m_angularVelocity * m_UpdateTimestep * 0.5f));
      obj->m_orientation.Normalise();

      break;
    }

    case INTEGRATION_RUNGE_KUTTA_4:
    {
      // RK4 integration for linear motion
      IntegrationHelpers::State state = {obj->m_position, obj->m_linearVelocity, obj->m_linearForce * obj->m_inverseMass};
      IntegrationHelpers::RK4(state, m_UpdateTimestep);
      obj->m_position = state.position;
      obj->m_linearVelocity = state.velocity;

      // Linear velocity damping
      obj->m_linearVelocity = obj->m_linearVelocity * damping;

      // Update angular velocity
      obj->m_angularVelocity += obj->m_inverseInertia * obj->m_torque * m_UpdateTimestep;

      // Angular velocity damping
      obj->m_angularVelocity = obj->m_angularVelocity * damping;

      // Update orientation
      obj->m_orientation = obj->m_orientation + (obj->m_orientation * (obj->m_angularVelocity * m_UpdateTimestep * 0.5f));
      obj->m_orientation.Normalise();

      break;
    }
    }

    // Mark cached world transform and AABB as invalid
    obj->m_wsTransformInvalidated = true;
    obj->m_wsAabbInvalidated = true;
  }

  // Test for rest conditions
  obj->DoAtRestTest();
}

/**
 * @brief Handle narrowphase collision detection.
 */
void PhysicsEngine::NarrowPhaseCollisions()
{
  if (m_BroadphaseCollisionPairs.size() > 0)
  {
    // Collision data to pass between detection and manifold generation stages.
    CollisionData colData;

    // Collision Detection Algorithm to use
    CollisionDetectionSAT colDetect;

    // Iterate over all possible collision pairs and perform accurate collision detection
    for (size_t i = 0; i < m_BroadphaseCollisionPairs.size(); ++i)
    {
      CollisionPair &cp = m_BroadphaseCollisionPairs[i];

      // Broadphase debug draw
      if (m_DebugDrawFlags & DEBUGDRAW_FLAGS_BROADPHASE_PAIRS)
      {
        NCLDebug::DrawThickLine(cp.pObjectA->GetPosition(), cp.pObjectB->GetPosition(), 0.02f, Vector4(0.0f, 0.0f, 1.0f, 1.0f));
        NCLDebug::DrawPointNDT(cp.pObjectA->GetPosition(), 0.05f, Vector4(0.0f, 1.0f, 0.5f, 1.0f));
        NCLDebug::DrawPointNDT(cp.pObjectB->GetPosition(), 0.05f, Vector4(0.0f, 1.0f, 0.5f, 1.0f));
      }

      for (auto aIt = cp.pObjectA->CollisionShapesBegin(); aIt != cp.pObjectA->CollisionShapesEnd(); ++aIt)
      {
        for (auto bIt = cp.pObjectB->CollisionShapesBegin(); bIt != cp.pObjectB->CollisionShapesEnd(); ++bIt)
        {
          ICollisionShape *shapeA = *aIt;
          ICollisionShape *shapeB = *bIt;

          colDetect.BeginNewPair(cp.pObjectA, cp.pObjectB, shapeA, shapeB);

          // Detects if the objects are colliding - Seperating Axis Theorem
          if (colDetect.AreColliding(&colData))
          {
            // Draw collision data to the window if requested
            // - Have to do this here as colData is only temporary.
            if (m_DebugDrawFlags & DEBUGDRAW_FLAGS_COLLISIONNORMALS)
            {
              NCLDebug::DrawPointNDT(colData._pointOnPlane, 0.1f, Vector4(0.5f, 0.5f, 1.0f, 1.0f));
              NCLDebug::DrawThickLineNDT(colData._pointOnPlane, colData._pointOnPlane - colData._normal * colData._penetration,
                                         0.05f, Vector4(0.0f, 0.0f, 1.0f, 1.0f));
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

              // Construct contact points that form the perimeter of the collision manifold
              colDetect.GenContactPoints(manifold);

              // Fire callback
              cp.pObjectA->FireOnCollisionManifoldCallback(cp.pObjectA, cp.pObjectB, manifold);
              cp.pObjectB->FireOnCollisionManifoldCallback(cp.pObjectB, cp.pObjectA, manifold);

              // Add to list of manifolds that need solving
              m_vpManifolds.push_back(manifold);
            }
          }
        }
      }
    }
  }
}

/**
 * @brief Draw visual debug information.
 */
void PhysicsEngine::DebugRender()
{
  // Draw all collision manifolds
  if (m_DebugDrawFlags & DEBUGDRAW_FLAGS_MANIFOLD)
  {
    for (Manifold *m : m_vpManifolds)
      m->DebugDraw();
  }

  // Draw all constraints
  if (m_DebugDrawFlags & DEBUGDRAW_FLAGS_CONSTRAINT)
  {
    for (IConstraint *c : m_vpConstraints)
      c->DebugDraw();
  }

  // Draw all objects and collision shapes
  for (PhysicsObject *obj : m_PhysicsObjects)
  {
    obj->DebugDraw(m_DebugDrawFlags);

    if (m_DebugDrawFlags & DEBUGDRAW_FLAGS_COLLISIONVOLUMES)
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

/**
 * @brief Finds a physics object based on the name of its associated (parent) game object.
 * @param name Name of the obeject to find
 * @return Pointer to physics object (nullptr if not found)
 */
PhysicsObject *PhysicsEngine::FindObjectByName(const std::string &name)
{
  auto it = std::find_if(m_PhysicsObjects.begin(), m_PhysicsObjects.end(), [name](PhysicsObject *o) {
    Object *po = o->GetAssociatedObject();
    return (po != nullptr && po->GetName() == name);
  });

  return (it == m_PhysicsObjects.end()) ? nullptr : *it;
}
