#include "ShootableCube.h"

#include <ncltech/CommonMeshes.h>
#include <ncltech/CuboidCollisionShape.h>

/**
 * @brief Creates a new shootable cube.
 * @param owner Player that shot the cube
 * @param lifetime TIme in seconds the object is expected to stay alive for
 * @param halfDims Half dimensions of the cube
 * @param inverseMass Inverse mass
 */
ShootableCube::ShootableCube(Player *owner, float lifetime, const Vector3 &halfDims, float inverseMass)
    : IShootable(owner, lifetime)
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
