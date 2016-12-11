#pragma once

#include "ICollisionShape.h"

#include "Hull.h"

#include <nclgl/Mesh.h>

class HullCollisionShape : public ICollisionShape
{
public:
  HullCollisionShape();
  virtual ~HullCollisionShape();

  void BuildFromMesh(Mesh *mesh);

  virtual Matrix3 BuildInverseInertia(float invMass) const override;

  virtual void GetCollisionAxes(const PhysicsObject *currentObject, std::vector<Vector3> *axes) const override;

  virtual void GetEdges(const PhysicsObject *currentObject, std::vector<CollisionEdge> *edges) const override;

  virtual void GetMinMaxVertexOnAxis(const PhysicsObject *currentObject, const Vector3 &axis, Vector3 *min,
                                     Vector3 *max) const override;

  virtual void GetIncidentReferencePolygon(const PhysicsObject *currentObject, const Vector3 &axis, std::list<Vector3> *face,
                                           Vector3 *normal, std::vector<Plane> *adjacentPlanes) const override;

  virtual void DebugDraw(const PhysicsObject *currentObject) const override;

protected:
  virtual void GetShapeWorldTransformation(const PhysicsObject *currentObject, Matrix4 &transform) const;

protected:
  Hull m_hull;
};
