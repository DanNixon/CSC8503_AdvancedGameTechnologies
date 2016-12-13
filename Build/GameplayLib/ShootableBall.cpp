#include "ShootableBall.h"

#include <ncltech/CommonMeshes.h>
#include <ncltech/SphereCollisionShape.h>

ShootableBall::ShootableBall(Player *owner, float radius, float inverseMass, float lifetime)
    : IShootable(owner, lifetime)
{
  SetMesh(CommonMeshes::Sphere(), false);
  SetLocalTransform(Matrix4::Scale(Vector3(radius, radius, radius)));

  ICollisionShape *shape = new SphereCollisionShape(radius);
  m_pPhysicsObject->AddCollisionShape(shape);
  m_pPhysicsObject->SetInverseInertia(shape->BuildInverseInertia(inverseMass));

  PostCreate(inverseMass);
}

ShootableBall::~ShootableBall()
{
}