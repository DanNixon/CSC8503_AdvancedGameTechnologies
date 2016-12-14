#pragma once

#include "IConstraint.h"
#include "NCLDebug.h"
#include "PhysicsEngine.h"

/**
 * @class WeldConstraint
 * @author Dan Nixon
 * @brief Constraint to keep two objects locked together.
 */
class WeldConstraint : public IConstraint
{
public:
  WeldConstraint(PhysicsObject *obj1, PhysicsObject *obj2);

  virtual void ApplyImpulse() override;
  virtual void DebugDraw() const;

protected:
  PhysicsObject *m_pObj1; //!< First (parent) object
  PhysicsObject *m_pObj2; //!< Second (child) object

  Vector3 m_positionOffset; //!< Position offset from first object to second
  Quaternion m_orientation; //!< Original orientation of the second object
};
