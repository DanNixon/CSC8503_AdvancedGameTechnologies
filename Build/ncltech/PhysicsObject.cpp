#include "PhysicsObject.h"
#include "PhysicsEngine.h"

PhysicsObject::PhysicsObject()
    : m_wsTransformInvalidated(true)
    , m_AtRest(false)
    , m_RestVelocityThresholdSquared(0.001f)
    , m_AverageSummedVelocity(0.0f)
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
 * Uses an exponential moving average of the sum of the magnitudes of linear and angular acceleration and sets the at
 * rest flag when this average is below a threshold.
 *
 * Probably not the best solution but is relatively cheap and reduces the likelyhood of an object entering rest state
 * when changing direction.
 */
void PhysicsObject::DoAtRestTest()
{
  // Negative threshold disables test, don't bother calculating average or performing test
  if (m_RestVelocityThresholdSquared <= 0.0f)
    return;

  // Vaue between 0 and 1, higher values discard old data faster
  const float alpha = 0.7f;

  // Calculate exponential moving average
  float v = m_LinearVelocity.LengthSquared() + m_AngularVelocity.LengthSquared();
  m_AverageSummedVelocity += alpha * (v - m_AverageSummedVelocity);

  // Do test (only if at rest flag is not already set)
  if (!m_AtRest && m_AverageSummedVelocity <= m_RestVelocityThresholdSquared)
    m_AtRest = true;
}
