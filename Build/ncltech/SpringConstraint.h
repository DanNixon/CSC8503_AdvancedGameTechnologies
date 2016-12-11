#pragma once

#include "IConstraint.h"
#include "NCLDebug.h"
#include "PhysicsEngine.h"

class SpringConstraint : public IConstraint
{
public:
  SpringConstraint(PhysicsObject *obj1, PhysicsObject *obj2, const Vector3 &globalOnA, const Vector3 &globalOnB,
                   float springConstant, float dampingFactor);

  virtual void ApplyImpulse() override;
  virtual void DebugDraw() const;

protected:
  PhysicsObject *m_pObj1;
  PhysicsObject *m_pObj2;

  float m_restDistance;
  float m_springConstant;
  float m_dampingFactor;

  Vector3 m_LocalOnA;
  Vector3 m_LocalOnB;
};