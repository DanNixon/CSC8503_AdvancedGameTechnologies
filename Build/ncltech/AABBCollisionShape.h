/******************************************************************************
Class: AABBCollisionShape
Author: Dan Nixon
*/ /////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CuboidCollisionShape.h"
#include "Hull.h"

class AABBCollisionShape : public CuboidCollisionShape
{
public:
  AABBCollisionShape();
  AABBCollisionShape(const Vector3 &halfdims);
  virtual ~AABBCollisionShape();

  virtual void GetCollisionAxes(const PhysicsObject *currentObject, std::vector<Vector3> *out_axes) const override;

protected:
  virtual void GetShapeWorldTransformation(const PhysicsObject *currentObject, Matrix4 &transform) const;
};
