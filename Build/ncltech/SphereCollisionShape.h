/******************************************************************************
Class: SphereCollisionShape
Implements: CollisionShape
Author: Pieran Marris      <p.marris@newcastle.ac.uk> and YOU!
Description:

Extends CollisionShape to represent a sphere.

A sphere object is one of the easiest 3D shapes to model, as for collision purposes it can only ever have one axis of itersection,
it also can only ever have one contact point. This makes alot of the returns quite trival compared to the cuboid that requires alot of
setting up and adjanency information.

       (\_/)
       ( '_')
     /""""""""""""\=========     -----D
    /"""""""""""""""""""""""\
....\_@____@____@____@____@_/

*/ /////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ICollisionShape.h"

/**
 * @class SphereCollisionShape
 * @brief Collision shape for a sphere.
 */
class SphereCollisionShape : public ICollisionShape
{
public:
  SphereCollisionShape(float radius = 1.0f);
  virtual ~SphereCollisionShape();

  /**
   * @brief Sets the sphere radius.
   * @param radius Radius
   */
  void SetRadius(float radius)
  {
    m_Radius = radius;
  }

  /**
   * @brief Gets the sphere radius.
   * @return Radius
   */
  inline float GetRadius() const
  {
    return m_Radius;
  }

  virtual void DebugDraw(const PhysicsObject *currentObject) const override;

  virtual Matrix3 BuildInverseInertia(float invMass) const override;

  virtual void GetCollisionAxes(const PhysicsObject *currentObject, std::vector<Vector3> *out_axes) const override;

  virtual void GetEdges(const PhysicsObject *currentObject, std::vector<CollisionEdge> *out_edges) const override;

  virtual void GetMinMaxVertexOnAxis(const PhysicsObject *currentObject, const Vector3 &axis, Vector3 *out_min,
                                     Vector3 *out_max) const override;

  virtual void GetIncidentReferencePolygon(const PhysicsObject *currentObject, const Vector3 &axis, std::list<Vector3> *out_face,
                                           Vector3 *out_normal, std::vector<Plane> *out_adjacent_planes) const override;

protected:
  float m_Radius; //!< Sphere radius
};
