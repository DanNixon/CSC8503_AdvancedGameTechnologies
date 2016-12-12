#pragma once

#include "CuboidCollisionShape.h"
#include "Hull.h"

/**
 * @class AABBCollisionShape
 * @author Dan Nixon
 * @brief Collision shape for an axis aligned bounding box.
 *
 * Essentially a cuboid collision shape with no rotation transformation.
 */
class AABBCollisionShape : public CuboidCollisionShape
{
public:
  AABBCollisionShape();
  AABBCollisionShape(const Vector3 &lowerLeft, const Vector3 &upperRight);
  virtual ~AABBCollisionShape();

  virtual void GetCollisionAxes(const PhysicsObject *currentObject, std::vector<Vector3> *out_axes) const override;

protected:
  virtual void GetShapeWorldTransformation(const PhysicsObject *currentObject, Matrix4 &transform) const;
};
