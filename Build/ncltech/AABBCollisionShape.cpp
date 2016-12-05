#include "AABBCollisionShape.h"
#include "PhysicsObject.h"
#include <nclgl/Matrix3.h>
#include <nclgl/OGLRenderer.h>

AABBCollisionShape::AABBCollisionShape()
    : CuboidCollisionShape()
{
}

AABBCollisionShape::AABBCollisionShape(const Vector3 &halfdims)
    : CuboidCollisionShape(halfdims)
{
}

AABBCollisionShape::~AABBCollisionShape()
{
}

void AABBCollisionShape::GetCollisionAxes(const PhysicsObject *currentObject, std::vector<Vector3> *axis) const
{
  if (axis)
  {
    axis->push_back(Vector3(1.0f, 0.0f, 0.0f)); // X
    axis->push_back(Vector3(0.0f, 1.0f, 0.0f)); // Y
    axis->push_back(Vector3(0.0f, 0.0f, 1.0f)); // Z
  }
}

void AABBCollisionShape::GetShapeWorldTransformation(const PhysicsObject *currentObject, Matrix4 &transform) const
{
  Matrix4 objectWs = currentObject->GetWorldSpaceTransform();
  objectWs.ClearRotation();
  transform = objectWs * m_LocalTransform * Matrix4::Scale(m_CuboidHalfDimensions);
}