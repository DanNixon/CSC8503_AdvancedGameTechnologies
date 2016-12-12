#include "PlaneCollisionShape.h"

#include "PhysicsObject.h"
#include "NCLDebug.h"

/**
 * @brief Create a new plane collision shape.
 * @param normal Plane normal
 */
PlaneCollisionShape::PlaneCollisionShape(const Vector3 &normal)
    : m_normal(normal)
{
}

PlaneCollisionShape::~PlaneCollisionShape()
{
}

/**
 * @copydoc ICollisionShape::BuildInverseInertia
 */
Matrix3 PlaneCollisionShape::BuildInverseInertia(float invMass) const
{
  // TODO
  return Matrix3();
}

/**
 * @copydoc ICollisionShape::GetCollisionAxes
 */
void PlaneCollisionShape::GetCollisionAxes(const PhysicsObject * currentObject, std::vector<Vector3>* out_axes) const
{
  // TODO
}

/**
 * @copydoc ICollisionShape::GetEdges
 */
void PlaneCollisionShape::GetEdges(const PhysicsObject * currentObject, std::vector<CollisionEdge>* out_edges) const
{
  // TODO
}

/**
 * @copydoc ICollisionShape::GetMinMaxVertexOnAxis
 */
void PlaneCollisionShape::GetMinMaxVertexOnAxis(const PhysicsObject * currentObject, const Vector3 & axis, Vector3 * out_min, Vector3 * out_max) const
{
  // TODO
}

/**
 * @copydoc ICollisionShape::GetIncidentReferencePolygon
 */
void PlaneCollisionShape::GetIncidentReferencePolygon(const PhysicsObject * currentObject, const Vector3 & axis, std::list<Vector3>* out_face, Vector3 * out_normal, std::vector<Plane>* out_adjacent_planes) const
{
  // TODO
}

/**
 * @copydoc ICollisionShape::DebugDraw
 */
void PlaneCollisionShape::DebugDraw(const PhysicsObject * currentObject) const
{
  Matrix4 transform = currentObject->GetWorldSpaceTransform() * m_LocalTransform;
  Vector3 pos = transform.GetPositionVector();

  // Draw normal
  static const Vector4 NORMAL_COLOUR(0.8f, 0.2f, 1.0f, 1.0f);
  NCLDebug::DrawPointNDT(pos, 0.05f, NORMAL_COLOUR);
  NCLDebug::DrawThickLineNDT(pos, pos + m_normal, 0.02f, NORMAL_COLOUR);

  // Draw plane bounds
  static const Vector4 PLANE_BOUNDS_COLOUR(0.1f, 1.0f, 0.2f, 1.0f);
  // TODO
  NCLDebug::DrawThickLineNDT(pos, pos + Vector3(1.0f, 0.0f, 0.0f), 0.02f, PLANE_BOUNDS_COLOUR);
  NCLDebug::DrawThickLineNDT(pos, pos + Vector3(0.0f, 0.0f, 1.0f), 0.02f, PLANE_BOUNDS_COLOUR);
}