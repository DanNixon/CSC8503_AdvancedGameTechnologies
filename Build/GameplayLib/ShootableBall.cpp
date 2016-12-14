#include "ShootableBall.h"

#include <ncltech/CommonMeshes.h>
#include <ncltech/SphereCollisionShape.h>

/**
 * @brief Creates a new shootable ball.
 * @param owner Player that shot the ball
 * @param radius Radius of the ball
 * @param inverseMass Inverse mass
 * @param lifetime TIme in seconds the object is expected to stay alive for
 */
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
