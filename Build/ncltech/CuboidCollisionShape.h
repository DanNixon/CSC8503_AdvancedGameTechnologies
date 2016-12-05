#pragma once

#include "AABB.h"
#include "ICollisionShape.h"

class CuboidCollisionShape : public ICollisionShape
{
public:
  CuboidCollisionShape(const Vector3 &halfDims = Vector3(0.5f, 0.5f, 0.5f));
  virtual ~CuboidCollisionShape();

  void SetHalfDims(const Vector3 &halfDims)
  {
    m_hull.SetHalfDimensions(halfDims);
  }

  void SetLowerLeft(const Vector3 &lowerLeft)
  {
    m_hull.SetLower(lowerLeft);
  }

  void SetUpperRight(const Vector3 &upperRight)
  {
    m_hull.SetUpper(upperRight);
  }

  Vector3 LowerLeft() const
  {
    return m_hull.GetLower();
  }

  Vector3 UpperRight() const
  {
    return m_hull.GetUpper();
  }

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
  AABB m_hull;
};
