#pragma once

#include "IConstraint.h"
#include "NCLDebug.h"
#include "PhysicsEngine.h"

class WeldConstraint : public IConstraint
{
public:
  WeldConstraint(PhysicsObject *obj1, PhysicsObject *obj2, const Vector3 &globalOnA, const Vector3 &globalOnB);

  virtual void ApplyImpulse() override;
  virtual void DebugDraw() const;

protected:
  PhysicsObject *m_pObj1;
  PhysicsObject *m_pObj2;

  Vector3 m_LocalOnA;
  Vector3 m_LocalOnB;
};