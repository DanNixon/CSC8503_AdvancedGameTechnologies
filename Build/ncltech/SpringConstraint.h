#pragma once

#include "IConstraint.h"
#include "NCLDebug.h"
#include "PhysicsEngine.h"

/**
 * @class SpringConstraint
 * @author Dan Nixon
 * @brief Constraint simulating a spring between two objects.
 */
class SpringConstraint : public IConstraint
{
public:
  SpringConstraint(PhysicsObject *obj1, PhysicsObject *obj2, const Vector3 &globalOnA, const Vector3 &globalOnB,
                   float springConstant, float dampingFactor);

  virtual void ApplyImpulse() override;
  virtual void DebugDraw() const;

protected:
  PhysicsObject *m_pObj1; //!< First object
  PhysicsObject *m_pObj2; //!< Second object

  float m_restDistance; //!< Distance between objects when spring is at rest

  float m_springConstant; //!< Spring constant
  float m_dampingFactor;  //!< Spring damping factor

  Vector3 m_LocalOnA; //!< Local constraint point transform on first object
  Vector3 m_LocalOnB; //!< Local constraint point transform on second object
};
