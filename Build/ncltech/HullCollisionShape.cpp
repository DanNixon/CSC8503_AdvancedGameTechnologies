#include "HullCollisionShape.h"
#include "PhysicsObject.h"
#include <nclgl/Matrix3.h>
#include <nclgl/OGLRenderer.h>
#include "NCLDebug.h"

HullCollisionShape::HullCollisionShape()
{
}

HullCollisionShape::~HullCollisionShape()
{
}

void HullCollisionShape::BuildFromMesh(Mesh * mesh)
{
  // Add vertices
  for (size_t i = 0; i < mesh->numVertices; i++)
    m_hull.AddVertex(mesh->vertices[i]);

  // Add faces
  switch (mesh->type)
  {
  case GL_TRIANGLES:
    if (mesh->numIndices == 0)
    {
      for (size_t i = 0; i < mesh->numVertices; i += 3)
      {
        Vector3 n1 = mesh->normals[i];
        Vector3 n2 = mesh->normals[i + 1];
        Vector3 n3 = mesh->normals[i + 2];

        Vector3 normal = n1 + n2 + n3;
        normal.Normalise();

        int vertexIdx[] = { (int)i, (int)i + 1, (int)i + 2 };
        m_hull.AddFace(normal, 3, vertexIdx);
      }
    }
    else
    {
      NCLERROR("Indexed triangles are not supported by HullCollisionShape!");
    }
    
    break;

  default:
    NCLERROR("Mesh type not supported by HullCollisionShape!");
  }
}

Matrix3 HullCollisionShape::BuildInverseInertia(float invMass) const
{
  BoundingBox bb = m_hull.GetBoundingBox();
  Vector3 dimsSq = bb.Upper() - bb.Lower();
  dimsSq = dimsSq * dimsSq;

  Matrix3 inertia;
  inertia._11 = 12.f * invMass / (dimsSq.y + dimsSq.z);
  inertia._22 = 12.f * invMass / (dimsSq.x + dimsSq.z);
  inertia._33 = 12.f * invMass / (dimsSq.x + dimsSq.y);

  return inertia;
}

void HullCollisionShape::GetCollisionAxes(const PhysicsObject *currentObject, std::vector<Vector3> *axes) const
{
  if (axes)
  {
    Matrix3 objOrientation = currentObject->GetOrientation().ToMatrix3();
    axes->push_back(objOrientation * Vector3(1.0f, 0.0f, 0.0f)); // X
    axes->push_back(objOrientation * Vector3(0.0f, 1.0f, 0.0f)); // Y
    axes->push_back(objOrientation * Vector3(0.0f, 0.0f, 1.0f)); // Z
  }
}

void HullCollisionShape::GetEdges(const PhysicsObject *currentObject, std::vector<CollisionEdge> *edges) const
{
  if (edges)
  {
    Matrix4 transform;
    GetShapeWorldTransformation(currentObject, transform);

    for (unsigned int i = 0; i < m_hull.GetNumEdges(); ++i)
    {
      const HullEdge &edge = m_hull.GetEdge(i);
      Vector3 A = transform * m_hull.GetVertex(edge.vStart).pos;
      Vector3 B = transform * m_hull.GetVertex(edge.vEnd).pos;

      edges->push_back(CollisionEdge(A, B));
    }
  }
}

void HullCollisionShape::GetMinMaxVertexOnAxis(const PhysicsObject *currentObject, const Vector3 &axis, Vector3 *min,
                                                 Vector3 *max) const
{
  // Build World Transform
  Matrix4 transform;

  if (currentObject == nullptr)
    transform = m_LocalTransform;
  else
    GetShapeWorldTransformation(currentObject, transform);

  // Convert world space axis into model space (Axis Aligned Cuboid)
  Matrix3 invNormalMatrix = Matrix3::Transpose(Matrix3(transform));
  Vector3 local_axis = invNormalMatrix * axis;
  local_axis.Normalise();

  // Get closest and furthest vertex id's
  int vMin, vMax;
  m_hull.GetMinMaxVerticesInAxis(local_axis, &vMin, &vMax);

  // Return closest and furthest vertices in world-space
  if (min)
    *min = transform * m_hull.GetVertex(vMin).pos;
  if (max)
    *max = transform * m_hull.GetVertex(vMax).pos;
}

void HullCollisionShape::GetIncidentReferencePolygon(const PhysicsObject *currentObject, const Vector3 &axis,
                                                       std::list<Vector3> *face, Vector3 *normal,
                                                       std::vector<Plane> *adjacentPlanes) const
{
  // Get the world-space transform
  Matrix4 transform;
  GetShapeWorldTransformation(currentObject, transform);

  // Get normal and inverse-normal matrices to transfom the collision axis to
  // and from modelspace
  Matrix3 invNormalMatrix = Matrix3::Transpose(Matrix3(transform));
  Matrix3 normalMatrix = Matrix3::Inverse(invNormalMatrix);

  Vector3 local_axis = invNormalMatrix * axis;

  // Get the furthest vertex along axis - this will be part of the further face
  int undefined, maxVertex;
  m_hull.GetMinMaxVerticesInAxis(local_axis, &undefined, &maxVertex);
  const HullVertex &vert = m_hull.GetVertex(maxVertex);

  // Compute which face (that contains the furthest vertex above)
  // is the furthest along the given axis. This is defined by
  // it's normal being closest to parallel with the collision axis.
  const HullFace *best_face = 0;
  float best_correlation = -FLT_MAX;
  for (int faceIdx : vert.enclosing_faces)
  {
    const HullFace *face = &m_hull.GetFace(faceIdx);
    float temp_correlation = Vector3::Dot(local_axis, face->_normal);
    if (temp_correlation > best_correlation)
    {
      best_correlation = temp_correlation;
      best_face = face;
    }
  }

  // Output face normal
  if (normal)
  {
    *normal = normalMatrix * best_face->_normal;
    (*normal).Normalise();
  }

  // Output face vertices (transformed back into world-space)
  if (face)
  {
    for (int vertIdx : best_face->vert_ids)
    {
      const HullVertex &vert = m_hull.GetVertex(vertIdx);
      face->push_back(transform * vert.pos);
    }
  }

  // Now, we need to define a set of planes that will clip any 3d geometry down
  // to fit inside
  // the shape. This results in us forming list of clip planes from each of the
  // adjacent faces along with the reference face itself.
  if (adjacentPlanes)
  {
    Vector3 wsPointOnPlane = transform * m_hull.GetVertex(m_hull.GetEdge(best_face->edge_ids[0]).vStart).pos;

    // First, form a plane around the reference face
    {
      // We use the negated normal here for the plane, as we want to clip
      // geometry left outside the shape not inside it.
      Vector3 planeNrml = -(normalMatrix * best_face->_normal);
      planeNrml.Normalise();

      float planeDist = -Vector3::Dot(planeNrml, wsPointOnPlane);
      adjacentPlanes->push_back(Plane(planeNrml, planeDist));
    }

    // Now we need to loop over all adjacent faces, and form a similar
    // clip plane around those too.
    // - The way that the HULL object is constructed means each edge can only
    //   ever have two adjoining faces. This means we can iterate through all
    //   edges of the face and then build a plane around the other face that
    //   also shares that edge.
    for (int edgeIdx : best_face->edge_ids)
    {
      const HullEdge &edge = m_hull.GetEdge(edgeIdx);

      wsPointOnPlane = transform * m_hull.GetVertex(edge.vStart).pos;

      for (int adjFaceIdx : edge.enclosing_faces)
      {
        if (adjFaceIdx != best_face->idx)
        {
          const HullFace &adjFace = m_hull.GetFace(adjFaceIdx);

          Vector3 planeNrml = -(normalMatrix * adjFace._normal);
          planeNrml.Normalise();
          float planeDist = -Vector3::Dot(planeNrml, wsPointOnPlane);

          adjacentPlanes->push_back(Plane(planeNrml, planeDist));
        }
      }
    }
  }
}

void HullCollisionShape::DebugDraw(const PhysicsObject *currentObject) const
{
  Matrix4 transform;
  GetShapeWorldTransformation(currentObject, transform);

  // Just draw the hull-mesh at the position of our PhysicsObject
  m_hull.DebugDraw(transform);
}

void HullCollisionShape::GetShapeWorldTransformation(const PhysicsObject *currentObject, Matrix4 &transform) const
{
  transform = currentObject->GetWorldSpaceTransform() * m_LocalTransform;
}