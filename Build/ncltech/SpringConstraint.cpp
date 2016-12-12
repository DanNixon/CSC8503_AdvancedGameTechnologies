#include "SpringConstraint.h"

/**
 * @brief Create a new spring constraint.
 * @param obj1 First object
 * @param obj2 Second object
 * @param globalOnA Position of constraint point on first object
 * @param globalOnB Position of constraint point on second object
 * @param springConstant Spring constant
 * @param dampingFactor Damping factor
 */
SpringConstraint::SpringConstraint(PhysicsObject *obj1, PhysicsObject *obj2, const Vector3 &globalOnA, const Vector3 &globalOnB,
                                   float springConstant, float dampingFactor)
    : m_pObj1(obj1)
    , m_pObj2(obj2)
    , m_springConstant(springConstant)
    , m_dampingFactor(dampingFactor)
{
  Vector3 ab = globalOnB - globalOnA;
  m_restDistance = ab.Length();

  Vector3 r1 = (globalOnA - m_pObj1->GetPosition());
  Vector3 r2 = (globalOnB - m_pObj2->GetPosition());
  m_LocalOnA = Matrix3::Transpose(m_pObj1->GetOrientation().ToMatrix3()) * r1;
  m_LocalOnB = Matrix3::Transpose(m_pObj2->GetOrientation().ToMatrix3()) * r2;
}

/**
 * @copydoc IConstraint::ApplyImpulse
 */
void SpringConstraint::ApplyImpulse()
{
  if (m_pObj1->GetInverseMass() + m_pObj2->GetInverseMass() == 0.0f)
    return;

  Vector3 r1 = m_pObj1->GetOrientation().ToMatrix3() * m_LocalOnA;
  Vector3 r2 = m_pObj2->GetOrientation().ToMatrix3() * m_LocalOnB;

  Vector3 globalOnA = r1 + m_pObj1->GetPosition();
  Vector3 globalOnB = r2 + m_pObj2->GetPosition();

  Vector3 ab = globalOnB - globalOnA;
  Vector3 abn = ab;
  abn.Normalise();

  Vector3 v0 = m_pObj1->GetLinearVelocity() + Vector3::Cross(m_pObj1->GetAngularVelocity(), r1);
  Vector3 v1 = m_pObj2->GetLinearVelocity() + Vector3::Cross(m_pObj2->GetAngularVelocity(), r2);
  float constraintMass = (m_pObj1->GetInverseMass() + m_pObj2->GetInverseMass()) +
                         Vector3::Dot(abn, Vector3::Cross(m_pObj1->GetInverseInertia() * Vector3::Cross(r1, abn), r1) +
                                               Vector3::Cross(m_pObj2->GetInverseInertia() * Vector3::Cross(r2, abn), r2));

  float b = 0.0f;
  {
    float distanceOffset = ab.Length() - m_restDistance;
    float baumgarteScalar = 0.1f;
    b = -(baumgarteScalar / PhysicsEngine::Instance()->GetDeltaTime()) * distanceOffset;
  }

  float jn = (-(Vector3::Dot(v0 - v1, abn) + b) * m_springConstant) - (m_dampingFactor * (v0 - v1).Length());
  jn /= constraintMass;

  m_pObj1->SetLinearVelocity(m_pObj1->GetLinearVelocity() + abn * (jn * m_pObj1->GetInverseMass()));
  m_pObj2->SetLinearVelocity(m_pObj2->GetLinearVelocity() - abn * (jn * m_pObj2->GetInverseMass()));

  m_pObj1->SetAngularVelocity(m_pObj1->GetAngularVelocity() + m_pObj1->GetInverseInertia() * Vector3::Cross(r1, abn * jn));
  m_pObj2->SetAngularVelocity(m_pObj2->GetAngularVelocity() - m_pObj2->GetInverseInertia() * Vector3::Cross(r2, abn * jn));
}

/**
 * @copydoc IConstraint::DebugDraw
 */
void SpringConstraint::DebugDraw() const
{
  Vector3 globalOnA = m_pObj1->GetOrientation().ToMatrix3() * m_LocalOnA + m_pObj1->GetPosition();
  Vector3 globalOnB = m_pObj2->GetOrientation().ToMatrix3() * m_LocalOnB + m_pObj2->GetPosition();

  NCLDebug::DrawThickLine(globalOnA, globalOnB, 0.02f, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  NCLDebug::DrawPointNDT(globalOnA, 0.05f, Vector4(1.0f, 0.8f, 1.0f, 1.0f));
  NCLDebug::DrawPointNDT(globalOnB, 0.05f, Vector4(1.0f, 0.8f, 1.0f, 1.0f));
}
