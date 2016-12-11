#pragma once

#include "Constraint.h"
#include "NCLDebug.h"
#include "PhysicsEngine.h"

class WeldConstraint : public Constraint
{
public:
  WeldConstraint(PhysicsObject *obj1, PhysicsObject *obj2, const Vector3 &globalOnA, const Vector3 &globalOnB)
      : m_pObj1(obj1)
      , m_pObj2(obj2)
  {
    Vector3 r1 = (globalOnA - m_pObj1->GetPosition());
    Vector3 r2 = (globalOnB - m_pObj2->GetPosition());
    m_LocalOnA = Matrix3::Transpose(m_pObj1->GetOrientation().ToMatrix3()) * r1;
    m_LocalOnB = Matrix3::Transpose(m_pObj2->GetOrientation().ToMatrix3()) * r2;
  }

  virtual void ApplyImpulse() override
  {
    if (m_pObj1->GetInverseMass() + m_pObj2->GetInverseMass() == 0.0f)
      return;

    // TODO
  }

  virtual void DebugDraw() const
  {
    Vector3 globalOnA = m_pObj1->GetOrientation().ToMatrix3() * m_LocalOnA + m_pObj1->GetPosition();
    Vector3 globalOnB = m_pObj2->GetOrientation().ToMatrix3() * m_LocalOnB + m_pObj2->GetPosition();

    NCLDebug::DrawThickLine(globalOnA, globalOnB, 0.02f, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    NCLDebug::DrawPointNDT(globalOnA, 0.05f, Vector4(1.0f, 0.8f, 1.0f, 1.0f));
    NCLDebug::DrawPointNDT(globalOnB, 0.05f, Vector4(1.0f, 0.8f, 1.0f, 1.0f));
  }

protected:
  PhysicsObject *m_pObj1;
  PhysicsObject *m_pObj2;

  Vector3 m_LocalOnA;
  Vector3 m_LocalOnB;
};