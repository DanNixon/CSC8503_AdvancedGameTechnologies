#include "PhysicsObject.h"
#include "NCLDebug.h"
#include "PhysicsEngine.h"

PhysicsObject::PhysicsObject()
    : m_parent(nullptr)
    , m_wsTransformInvalidated(true)
    , m_wsAabbInvalidated(true)
    , m_atRest(false)
    , m_restVelocityThresholdSquared(0.001f)
    , m_averageSummedVelocity(0.0f)
    , m_gravitationTarget(nullptr)
    , m_localBoundingBox()
    , m_position(0.0f, 0.0f, 0.0f)
    , m_linearVelocity(0.0f, 0.0f, 0.0f)
    , m_linearForce(0.0f, 0.0f, 0.0f)
    , m_inverseMass(0.0f)
    , m_orientation(0.0f, 0.0f, 0.0f, 1.0f)
    , m_angularVelocity(0.0f, 0.0f, 0.0f)
    , m_torque(0.0f, 0.0f, 0.0f)
    , m_inverseInertia(Matrix3::ZeroMatrix)
    , m_friction(0.5f)
    , m_elasticity(0.9f)
    , m_onCollisionCallback(nullptr)
{
  m_localBoundingBox.SetHalfDimensions(Vector3(0.5f, 0.5f, 0.5f));
}

PhysicsObject::~PhysicsObject()
{
  // Delete collision shapes
  for (auto it = m_collisionShapes.begin(); it != m_collisionShapes.end(); ++it)
    delete *it;

  m_collisionShapes.clear();
}

BoundingBox PhysicsObject::GetWorldSpaceAABB() const
{
  if (m_wsAabbInvalidated)
  {
    m_wsAabb = m_localBoundingBox.Transform(GetWorldSpaceTransform());
    m_wsAabbInvalidated = false;
  }

  return m_wsAabb;
}

const Matrix4 &PhysicsObject::GetWorldSpaceTransform() const
{
  if (m_wsTransformInvalidated)
  {
    m_wsTransform = m_orientation.ToMatrix4();
    m_wsTransform.SetPositionVector(m_position);

    m_wsTransformInvalidated = false;
  }

  return m_wsTransform;
}

void PhysicsObject::AutoResizeBoundingBox()
{
  m_localBoundingBox.Reset();

  const Vector3 xAxis(1.0f, 0.0f, 0.0f);
  const Vector3 yAxis(0.0f, 1.0f, 0.0f);
  const Vector3 zAxis(0.0f, 0.0f, 1.0f);

  Vector3 lower, upper;

  for (auto it = m_collisionShapes.begin(); it != m_collisionShapes.end(); ++it)
  {
    (*it)->GetMinMaxVertexOnAxis(nullptr, xAxis, &lower, &upper);
    m_localBoundingBox.ExpandToFit(lower);
    m_localBoundingBox.ExpandToFit(upper);

    (*it)->GetMinMaxVertexOnAxis(nullptr, yAxis, &lower, &upper);
    m_localBoundingBox.ExpandToFit(lower);
    m_localBoundingBox.ExpandToFit(upper);

    (*it)->GetMinMaxVertexOnAxis(nullptr, zAxis, &lower, &upper);
    m_localBoundingBox.ExpandToFit(lower);
    m_localBoundingBox.ExpandToFit(upper);
  }

  m_wsAabbInvalidated = true;
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
  if (m_restVelocityThresholdSquared <= 0.0f)
    return;

  // Value between 0 and 1, higher values discard old data faster
  const float alpha = 0.7f;

  // Calculate exponential moving average
  float v = m_linearVelocity.LengthSquared() + m_angularVelocity.LengthSquared();
  m_averageSummedVelocity += alpha * (v - m_averageSummedVelocity);

  // Do test
  m_atRest = m_averageSummedVelocity <= m_restVelocityThresholdSquared;
}

void PhysicsObject::DebugDraw(uint64_t flags) const
{
  if (flags & DEBUGDRAW_FLAGS_AABB)
    GetWorldSpaceAABB().DebugDraw(Matrix4(), Vector4(0.8f, 1.0f, 1.0f, 0.25f), Vector4(0.4f, 0.8f, 1.0f, 1.0f));

  if (flags & DEBUGDRAW_FLAGS_LINEARVELOCITY)
    NCLDebug::DrawThickLineNDT(m_wsTransform.GetPositionVector(), m_wsTransform * m_linearVelocity, 0.02f,
                               Vector4(0.0f, 1.0f, 0.0f, 1.0f));

  if (flags & DEBUGDRAW_FLAGS_LINEARFORCE)
    NCLDebug::DrawThickLineNDT(m_wsTransform.GetPositionVector(), m_wsTransform * m_linearForce, 0.02f,
                               Vector4(0.0f, 0.0f, 1.0f, 1.0f));
}