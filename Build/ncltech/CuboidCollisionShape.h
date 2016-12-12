#pragma once

#include "BoundingBoxHull.h"
#include "ICollisionShape.h"

/**
 * @class CuboidCollisionShape
 * @brief Collision shape for a cuboid.
 */
class CuboidCollisionShape : public ICollisionShape
{
public:
  CuboidCollisionShape(const Vector3 &halfDims = Vector3(0.5f, 0.5f, 0.5f));
  virtual ~CuboidCollisionShape();

  /**
   * @brief Sets the dimensions of the cuboid.
   * @param halfDims Half dimensions
   */
  void SetHalfDims(const Vector3 &halfDims)
  {
    m_hull.SetHalfDimensions(halfDims);
    m_hull.UpdateHull();
  }

  /**
   * @brief Sets the lower vertex of the cuboid.
   * @param lowerLeft Lower vertex
   */
  void SetLowerLeft(const Vector3 &lowerLeft)
  {
    m_hull.Lower() = lowerLeft;
    m_hull.UpdateHull();
  }

  /**
   * @brief Sets the upper vertex of the cuboid.
   * @param upperRight Upper vertex
   */
  void SetUpperRight(const Vector3 &upperRight)
  {
    m_hull.Upper() = upperRight;
    m_hull.UpdateHull();
  }

  /**
   * @brief Gets the lower vertex of the cuboid.
   * @return Lower vertex
   */
  Vector3 LowerLeft() const
  {
    return m_hull.Lower();
  }

  /**
   * @brief Gets the upper vertex of the cuboid.
   * @return Upper vertex
   */
  Vector3 UpperRight() const
  {
    return m_hull.Upper();
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
  BoundingBoxHull m_hull; //!< Hull describing cuboid
};
