#include "ShootableCube.h"

#include <ncltech/CommonMeshes.h>
#include <ncltech/CuboidCollisionShape.h>

ShootableCube::ShootableCube(const Vector3 &halfDims, float inverseMass, float lifetime)
    : IShootable(lifetime)
{
  SetMesh(CommonMeshes::Cube(), false);
  SetLocalTransform(Matrix4::Scale(halfDims));

  ICollisionShape *shape = new CuboidCollisionShape(halfDims);
  m_pPhysicsObject->AddCollisionShape(shape);
  m_pPhysicsObject->SetInverseInertia(shape->BuildInverseInertia(inverseMass));

  PostCreate(inverseMass);
}

ShootableCube::~ShootableCube()
{
}