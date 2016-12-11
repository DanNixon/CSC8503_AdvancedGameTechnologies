#pragma once

#include "IConstraint.h"
#include "NCLDebug.h"
#include "PhysicsEngine.h"

class WeldConstraint : public IConstraint
{
public:
  WeldConstraint(PhysicsObject *obj1, PhysicsObject *obj2);

  virtual void ApplyImpulse() override;
  virtual void DebugDraw() const;

protected:
  PhysicsObject *m_pObj1;
  PhysicsObject *m_pObj2;

  Vector3 m_positionOffset;
};