#include "WeldConstraint.h"

WeldConstraint::WeldConstraint(PhysicsObject *obj1, PhysicsObject *obj2)
    : m_pObj1(obj1)
    , m_pObj2(obj2)
{
  m_positionOffset = obj2->GetPosition() - obj1->GetPosition();
}

void WeldConstraint::ApplyImpulse()
{
  // Position
  Vector3 pos(m_positionOffset);
  Quaternion::RotatePointByQuaternion(m_pObj1->GetOrientation(), pos);
  pos += m_pObj1->GetPosition();

  // Orientation
  // TODO

  m_pObj2->SetPosition(pos);
}

void WeldConstraint::DebugDraw() const
{
  Vector3 posA = m_pObj1->GetPosition();
  Vector3 posB = m_pObj2->GetPosition();

  NCLDebug::DrawThickLine(posA, posB, 0.02f, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  NCLDebug::DrawPointNDT(posA, 0.05f, Vector4(1.0f, 0.8f, 1.0f, 1.0f));
  NCLDebug::DrawPointNDT(posB, 0.05f, Vector4(1.0f, 0.8f, 1.0f, 1.0f));
}
