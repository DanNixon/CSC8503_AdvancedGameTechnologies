#include "AABBCollisionShape.h"
#include "PhysicsObject.h"

/**
 * @brief Create a new AABB collision shape.
 */
AABBCollisionShape::AABBCollisionShape()
    : CuboidCollisionShape()
{
}

/**
 * @brief Create a new AABB collision shape with given dimensions.
 * @param lowerLeft Lower vertex
 * @param upperRight Upper vertex
 */
AABBCollisionShape::AABBCollisionShape(const Vector3 &lowerLeft, const Vector3 &upperRight)
    : CuboidCollisionShape()
{
  SetLowerLeft(lowerLeft);
  SetUpperRight(upperRight);
}

AABBCollisionShape::~AABBCollisionShape()
{
}

/**
 * @copydoc CuboidCollisionShape::GetCollisionAxes
 */
void AABBCollisionShape::GetCollisionAxes(const PhysicsObject *currentObject, std::vector<Vector3> *axis) const
{
  if (axis)
  {
    axis->push_back(Vector3(1.0f, 0.0f, 0.0f)); // X
    axis->push_back(Vector3(0.0f, 1.0f, 0.0f)); // Y
    axis->push_back(Vector3(0.0f, 0.0f, 1.0f)); // Z
  }
}

/**
 * @copydoc CuboidCollisionShape::GetShapeWorldTransformation
 */
void AABBCollisionShape::GetShapeWorldTransformation(const PhysicsObject *currentObject, Matrix4 &transform) const
{
  Matrix4 objectWs = currentObject->GetWorldSpaceTransform();
  objectWs.ClearRotation();
  transform = objectWs * m_LocalTransform;
}
