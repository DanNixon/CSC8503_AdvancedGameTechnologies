#include "PlaneCollisionShape.h"

#include "NCLDebug.h"
#include "PhysicsObject.h"

/**
 * @brief Create a new plane collision shape.
 * @param dimensions Dimensions of the 2D plane
 */
PlaneCollisionShape::PlaneCollisionShape(const Vector2 &dimensions)
  : m_dimensions(dimensions)
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
 *
 * There is only ever one possible face on a plane, only the plane normal is returned.
 */
void PlaneCollisionShape::GetCollisionAxes(const PhysicsObject *currentObject, std::vector<Vector3> *out_axes) const
{
  if (out_axes != nullptr)
  {
    Matrix4 transform = currentObject->GetWorldSpaceTransform() * m_LocalTransform;
    out_axes->push_back(-transform.GetBackVector());
  }
}

/**
 * @copydoc ICollisionShape::GetEdges
 */
void PlaneCollisionShape::GetEdges(const PhysicsObject *currentObject, std::vector<CollisionEdge> *out_edges) const
{
  Matrix4 transform;

  if (currentObject == nullptr)
    transform = m_LocalTransform;
  else
    transform = currentObject->GetWorldSpaceTransform() * m_LocalTransform;

  Vector3 pos = transform.GetPositionVector();

  Vector3 v1 = pos + (transform.GetRightVector() * -m_dimensions[0]) + (transform.GetUpVector() * m_dimensions[1]);
  Vector3 v2 = pos + (transform.GetRightVector() * m_dimensions[0]) + (transform.GetUpVector() * m_dimensions[1]);
  Vector3 v3 = pos + (transform.GetRightVector() * m_dimensions[0]) + (transform.GetUpVector() * -m_dimensions[1]);
  Vector3 v4 = pos + (transform.GetRightVector() * -m_dimensions[0]) + (transform.GetUpVector() * -m_dimensions[1]);

  if (out_edges != nullptr)
  {
    out_edges->push_back(CollisionEdge(v1, v2));
    out_edges->push_back(CollisionEdge(v2, v3));
    out_edges->push_back(CollisionEdge(v3, v4));
    out_edges->push_back(CollisionEdge(v4, v1));
  }
}

/**
 * @copydoc ICollisionShape::GetMinMaxVertexOnAxis
 */
void PlaneCollisionShape::GetMinMaxVertexOnAxis(const PhysicsObject *currentObject, const Vector3 &axis, Vector3 *out_min,
                                                Vector3 *out_max) const
{
  float minCorrelation = FLT_MAX;
  float maxCorrelation = -FLT_MAX;

  std::vector<CollisionEdge> edges;
  GetEdges(currentObject, &edges);

  for (auto it = edges.begin(); it != edges.end(); ++it)
  {
    float correlation = Vector3::Dot(axis, it->_v0);

    if (correlation > maxCorrelation)
    {
      maxCorrelation = correlation;

      if (out_max != nullptr)
        *out_max = it->_v0;
    }

    if (correlation <= minCorrelation)
    {
      minCorrelation = correlation;

      if (out_min != nullptr)
        *out_min = it->_v0;
    }
  }
}

/**
 * @copydoc ICollisionShape::GetIncidentReferencePolygon
 *
 * There is only ever one possible face on a plane, the plane normal and face are returned in all cases.
 */
void PlaneCollisionShape::GetIncidentReferencePolygon(const PhysicsObject *currentObject, const Vector3 &axis,
                                                      std::list<Vector3> *out_face, Vector3 *out_normal,
                                                      std::vector<Plane> *out_adjacent_planes) const
{
  Matrix4 transform = currentObject->GetWorldSpaceTransform() * m_LocalTransform;

  std::vector<CollisionEdge> edges;
  GetEdges(currentObject, &edges);

  // Normal
  if (out_normal != nullptr)
    *out_normal = -transform.GetBackVector();

  // Face
  if (out_face != nullptr)
  {
    for (auto it = edges.begin(); it != edges.end(); ++it)
      out_face->push_back(it->_v0);
  }

  // (Fake) adjacent planes
  if (out_adjacent_planes != nullptr)
  {
    out_adjacent_planes->push_back(Plane(-transform.GetUpVector(), -Vector3::Dot(-transform.GetUpVector(), edges[3]._v0)));
    out_adjacent_planes->push_back(Plane(transform.GetUpVector(), -Vector3::Dot(transform.GetUpVector(), edges[1]._v0)));
    out_adjacent_planes->push_back(Plane(-transform.GetRightVector(), -Vector3::Dot(-transform.GetRightVector(), edges[3]._v0)));
    out_adjacent_planes->push_back(Plane(transform.GetRightVector(), -Vector3::Dot(transform.GetRightVector(), edges[1]._v0)));
  }
}

/**
 * @copydoc ICollisionShape::DebugDraw
 */
void PlaneCollisionShape::DebugDraw(const PhysicsObject *currentObject) const
{
  static const Vector4 PLANE_COLOUR(0.1f, 1.0f, 0.2f, 1.0f);
  static const Vector4 NORMAL_COLOUR(1.0f, 0.2f, 0.2f, 1.0f);

  Matrix4 transform = currentObject->GetWorldSpaceTransform() * m_LocalTransform;
  Vector3 pos = transform.GetPositionVector();

  // Draw origin
  NCLDebug::DrawThickLineNDT(pos, pos + transform.GetUpVector(), 0.02f, PLANE_COLOUR);
  NCLDebug::DrawThickLineNDT(pos, pos + transform.GetRightVector(), 0.02f, PLANE_COLOUR);

  // Draw "normal"
  NCLDebug::DrawThickLineNDT(pos, pos + transform.GetBackVector(), 0.02f, NORMAL_COLOUR);

  // Draw bounds
  std::vector<CollisionEdge> edges;
  GetEdges(currentObject, &edges);
  for (auto it = edges.begin(); it != edges.end(); ++it)
    NCLDebug::DrawThickLineNDT(it->_v0, it->_v1, 0.02f, PLANE_COLOUR);
}