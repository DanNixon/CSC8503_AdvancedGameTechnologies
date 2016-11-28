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

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Constraint.h"
#include "NCLDebug.h"
#include "PhysicsEngine.h"

class DistanceConstraint : public Constraint
{
public:
	DistanceConstraint(PhysicsObject* obj1, PhysicsObject* obj2,
		const Vector3& globalOnA, const Vector3& globalOnB)
	{
		m_pObj1 = obj1;
		m_pObj2 = obj2;

		Vector3 ab = globalOnB - globalOnA;
		m_Distance = ab.Length();

		Vector3 r1 = (globalOnA - m_pObj1->GetPosition());
		Vector3 r2 = (globalOnB - m_pObj2->GetPosition());
		m_LocalOnA = Matrix3::Transpose(m_pObj1->GetOrientation().ToMatrix3()) * r1;
		m_LocalOnB = Matrix3::Transpose(m_pObj2->GetOrientation().ToMatrix3()) * r2;
	}

	virtual void ApplyImpulse() override
	{
		/* TUT 3 */
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
	PhysicsObject *m_pObj1, *m_pObj2;
	float   m_Distance;
	Vector3 m_LocalOnA;
	Vector3 m_LocalOnB;
};