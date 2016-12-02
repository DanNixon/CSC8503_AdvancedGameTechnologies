#include "PhysicsObject.h"
#include "PhysicsEngine.h"

PhysicsObject::PhysicsObject()
    : m_wsTransformInvalidated(true)
    , m_AtRest(false)
    , m_RestVelocityThresholdSquared(0.001f)
    , m_Position(0.0f, 0.0f, 0.0f)
    , m_LinearVelocity(0.0f, 0.0f, 0.0f)
    , m_Force(0.0f, 0.0f, 0.0f)
    , m_InvMass(0.0f)
    , m_Orientation(0.0f, 0.0f, 0.0f, 1.0f)
    , m_AngularVelocity(0.0f, 0.0f, 0.0f)
    , m_Torque(0.0f, 0.0f, 0.0f)
    , m_InvInertia(Matrix3::ZeroMatrix)
    , m_Friction(0.5f)
    , m_Elasticity(0.9f)
    , m_OnCollisionCallback(nullptr)
{
}

PhysicsObject::~PhysicsObject()
{
  // Delete collision shapes
  for (auto it = m_vCollisionShapes.begin(); it != m_vCollisionShapes.end(); ++it)
    delete *it;

  m_vCollisionShapes.clear();
}

const Matrix4 &PhysicsObject::GetWorldSpaceTransform() const
{
  if (m_wsTransformInvalidated)
  {
    m_wsTransform = m_Orientation.ToMatrix4();
    m_wsTransform.SetPositionVector(m_Position);

    m_wsTransformInvalidated = false;
  }

  return m_wsTransform;
}

/**
 * @brief Performs test to determine if this object is at rest and sets the at rest flag accordingly.
 *
 * Test finishes early if:
 *  - At rest flag is already set
 *  - Velocity threshold is negative (i.e. test is disabled)
 */
void PhysicsObject::DoAtRestTest()
{
  if (m_AtRest || m_RestVelocityThresholdSquared <= 0.0f)
    return;

  bool testResult = m_LinearVelocity.LengthSquared() < m_RestVelocityThresholdSquared &&
                    m_AngularVelocity.LengthSquared() < m_RestVelocityThresholdSquared;

  if (testResult)
    m_AtRest = true;
}
