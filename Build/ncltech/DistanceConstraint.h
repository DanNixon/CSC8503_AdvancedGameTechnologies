/******************************************************************************
Class: DistanceConstraint
Implements:
Author: Pieran Marris      <p.marris@newcastle.ac.uk> and YOU!
Description:

Manages a distance constraint between two objects, ensuring the two objects never
seperate. It works on a velocity level, enforcing the constraint:
	dot([(velocity of B) - (velocity of A)], normal) = zero

Thus ensuring that after integrating the position through the time, the distance between
the two objects never changes.

		(\_/)
		( '_')
	 /""""""""""""\=========     -----D
	/"""""""""""""""""""""""\
....\_@____@____@____@____@_/

*/ /////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IConstraint.h"
#include "NCLDebug.h"
#include "PhysicsEngine.h"

/**
 * @class DistanceConstraint
 * @brief Constraint to keep two objects a given distance apart.
 */
class DistanceConstraint : public IConstraint
{
public:
  DistanceConstraint(PhysicsObject *obj1, PhysicsObject *obj2, const Vector3 &globalOnA, const Vector3 &globalOnB);

  virtual void ApplyImpulse() override;
  virtual void DebugDraw() const;

protected:
  PhysicsObject *m_pObj1; //!< First object
  PhysicsObject *m_pObj2; //!< Second object

  float m_Distance; //!< target distance between objects

  Vector3 m_LocalOnA; //!< Local constraint point transform on first object
  Vector3 m_LocalOnB; //!< Local constraint point transform on second object
};
