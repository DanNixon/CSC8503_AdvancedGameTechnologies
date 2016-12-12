#pragma once

#include "ICollisionShape.h"

/**
 * @class PlaneCollisionShape
 * @author Dan Nixon
 * @brief Collision shape for a bounded plane.
 */
class PlaneCollisionShape : public ICollisionShape
{
public:
  PlaneCollisionShape(const Vector3 &normal = Vector3(0.0f, 1.0f, 0.0f));
  virtual ~PlaneCollisionShape();

  virtual Matrix3 BuildInverseInertia(float invMass) const override;

  virtual void GetCollisionAxes(const PhysicsObject *currentObject, std::vector<Vector3> *out_axes) const override;

  virtual void GetEdges(const PhysicsObject *currentObject, std::vector<CollisionEdge> *out_edges) const override;

  virtual void GetMinMaxVertexOnAxis(const PhysicsObject *currentObject, const Vector3 &axis, Vector3 *out_min,
                                     Vector3 *out_max) const override;

  virtual void GetIncidentReferencePolygon(const PhysicsObject *currentObject, const Vector3 &axis, std::list<Vector3> *out_face,
                                           Vector3 *out_normal, std::vector<Plane> *out_adjacent_planes) const override;

  virtual void DebugDraw(const PhysicsObject *currentObject) const override;

protected:
  Vector3 m_normal; //!< Plane normal
};
