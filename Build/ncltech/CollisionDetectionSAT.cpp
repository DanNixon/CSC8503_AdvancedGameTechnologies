#include "CollisionDetectionSAT.h"
#include "NCLDebug.h"

CollisionDetectionSAT::CollisionDetectionSAT()
{
}

void CollisionDetectionSAT::BeginNewPair(PhysicsObject *obj1, PhysicsObject *obj2, CollisionShape *shape1,
                                         CollisionShape *shape2)
{
  m_vPossibleCollisionAxes.clear();

  m_pObj1 = obj1;
  m_pObj2 = obj2;
  m_pShape1 = shape1;
  m_pShape2 = shape2;

  m_Colliding = false;
}

bool CollisionDetectionSAT::AreColliding(CollisionData *out_coldata)
{
  if (!m_pShape1 || !m_pShape2)
    return false;

  m_Colliding = false;

  FindAllPossibleCollisionAxes();

  m_BestColData._penetration = -FLT_MAX;

  CollisionData cur_colData;
  for (const Vector3 &axis : m_vPossibleCollisionAxes)
  {
    if (!CheckCollisionAxis(axis, &cur_colData))
      return false;

    if (cur_colData._penetration >= m_BestColData._penetration)
      m_BestColData = cur_colData;
  }

  if (out_coldata)
    *out_coldata = m_BestColData;

  m_Colliding = true;
  return true;
}

void CollisionDetectionSAT::FindAllPossibleCollisionAxes()
{
  m_pShape1->GetCollisionAxes(m_pObj1, &m_vPossibleCollisionAxes);
  m_pShape2->GetCollisionAxes(m_pObj2, &m_vPossibleCollisionAxes);

  std::vector<CollisionEdge> shape1_edges;
  std::vector<CollisionEdge> shape2_edges;

  m_pShape1->GetEdges(m_pObj1, &shape1_edges);
  m_pShape2->GetEdges(m_pObj2, &shape2_edges);

  // Handle special case for curved shapes
  bool shape1_isSphere = shape1_edges.empty();
  bool shape2_isSphere = shape2_edges.empty();

  if (shape1_isSphere && shape2_isSphere)
  {
    // TODO
    Vector3 axis = m_pObj2->GetPosition() - m_pObj1->GetPosition();
    axis.Normalise();
    AddPossibleCollisionAxis(axis);
  }
  else if (shape1_isSphere)
  {
    // TODO
    Vector3 p = GetClosestPoint(m_pObj1->GetPosition(), shape2_edges);
    Vector3 pt = m_pObj1->GetPosition() - p;
    pt.Normalise();
    AddPossibleCollisionAxis(pt);
  }
  else if (shape2_isSphere)
  {
    // TODO
    Vector3 p = GetClosestPoint(m_pObj2->GetPosition(), shape1_edges);
    Vector3 pt = m_pObj2->GetPosition() - p;
    pt.Normalise();
    AddPossibleCollisionAxis(pt);
  }
}

bool CollisionDetectionSAT::CheckCollisionAxis(const Vector3 &axis, CollisionData *coldata)
{
  Vector3 min1, min2, max1, max2;

  // Get the min /max vertices along the axis from shape1 and shape2
  m_pShape1->GetMinMaxVertexOnAxis(m_pObj1, axis, &min1, &max1);
  m_pShape2->GetMinMaxVertexOnAxis(m_pObj2, axis, &min2, &max2);

  float minCorrelation1 = Vector3::Dot(axis, min1);
  float maxCorrelation1 = Vector3::Dot(axis, max1);
  float minCorrelation2 = Vector3::Dot(axis, min2);
  float maxCorrelation2 = Vector3::Dot(axis, max2);

  // Object 1 mostly overlapping Object 2
  if (minCorrelation1 <= minCorrelation2 && maxCorrelation1 >= minCorrelation2)
  {
    if (coldata != NULL)
    {
      coldata->_normal = axis;
      coldata->_penetration = minCorrelation2 - maxCorrelation1;
      coldata->_pointOnPlane = max1 + coldata->_normal * coldata->_penetration;
    }

    return true;
  }

  // Object 2 mostly overlapping Object 1
  if (minCorrelation2 <= minCorrelation1 && maxCorrelation2 >= minCorrelation1)
  {
    if (coldata != NULL)
    {
      coldata->_normal = -axis;
      coldata->_penetration = minCorrelation1 - maxCorrelation2;
      coldata->_pointOnPlane = min1 + coldata->_normal * coldata->_penetration;
    }
    return true;
  }

  return false;
}

bool CollisionDetectionSAT::AddPossibleCollisionAxis(Vector3 axis)
{
  const float epsilon = 1e-6f;

  // is axis 0,0,0??
  if (Vector3::Dot(axis, axis) < epsilon)
    return false;

  axis.Normalise();

  for (const Vector3 &p_axis : m_vPossibleCollisionAxes)
  {
    // Is axis very close to the same as a previous axis already in the list of
    // axes??
    if (Vector3::Dot(axis, p_axis) >= 1.0f - epsilon)
      return false;
  }

  m_vPossibleCollisionAxes.push_back(axis);
  return true;
}

Vector3 CollisionDetectionSAT::GetClosestPoint(const Vector3 &pos, std::vector<CollisionEdge> &edges)
{
  Vector3 final_closest_point, edge_closest_point;
  float final_closest_distsq = FLT_MAX;

  for (const CollisionEdge &edge : edges)
  {
    // As we have a line not two points, the final value could be anywhere
    // between the edge points A/B
    //	To solve this by projecting the point (pos) onto the line described by
    // the edge, this is very similar
    //	to the means we use to test each axis in SAT except the axis in this
    // case is an edge which must be clamped
    //	between points A/B.
    Vector3 a_p = pos - edge._v0;
    Vector3 a_b = edge._v1 - edge._v0;

    // Distance along the line of point 'pos' in world distance
    float ABAPproduct = Vector3::Dot(a_p, a_b);
    float magnitudeAB = Vector3::Dot(a_b, a_b);

    // Distance along the line of point 'pos' between 0-1 where 0 is edgeA and 1
    // is edgeB
    float distance = ABAPproduct / magnitudeAB;

    // Clamp distance so it cant go beyond edgeA or edgeB in either direction
    distance = max(min(distance, 1.0f), 0.0f);

    edge_closest_point = edge._v0 + a_b * distance;

    // Only store the closest point if it's closer than the results returned
    // from previous edges.
    Vector3 c_t = pos - edge_closest_point;
    float temp_distsq = Vector3::Dot(c_t, c_t);

    if (temp_distsq < final_closest_distsq)
    {
      final_closest_distsq = temp_distsq;
      final_closest_point = edge_closest_point;
    }
  }

  return final_closest_point;
}

void CollisionDetectionSAT::GenContactPoints(Manifold *out_manifold)
{
  if (!out_manifold || !m_Colliding)
    return;

  std::list<Vector3> polygon1, polygon2;
  Vector3 normal1, normal2;
  std::vector<Plane> adjPlanes1, adjPlanes2;

  m_pShape1->GetIncidentReferencePolygon(m_pObj1, m_BestColData._normal, &polygon1, &normal1, &adjPlanes1);
  m_pShape2->GetIncidentReferencePolygon(m_pObj2, -m_BestColData._normal, &polygon2, &normal2, &adjPlanes2);

  if (polygon1.empty() && polygon2.empty())
  {
    // Empty manifold
    return;
  }
  else if (polygon1.size() == 1)
  {
    // Single contact point
    out_manifold->AddContact(polygon1.front(), polygon1.front() - m_BestColData._normal * m_BestColData._penetration,
                             m_BestColData._normal, m_BestColData._penetration);
  }
  else if (polygon2.size() == 1)
  {
    // Single contact point
    out_manifold->AddContact(polygon2.front(), polygon2.front() - m_BestColData._normal * m_BestColData._penetration,
                             m_BestColData._normal, m_BestColData._penetration);
  }
  else
  {
    std::list<Vector3> *incPolygon;
    std::list<Vector3> *refPolygon;
    Vector3 *incNormal;
    Vector3 *refNormal;
    std::vector<Plane> *refAdjPlanes;

    // Determine which polygon is the reference and incident
    bool flipped =
        fabs(Vector3::Dot(m_BestColData._normal, normal1)) <= fabs(Vector3::Dot(m_BestColData._normal, normal2));

    if (flipped)
    {
      refPolygon = &polygon2;
      refNormal = &normal2;
      incPolygon = &polygon1;
      incNormal = &normal1;

      refAdjPlanes = &adjPlanes2;
    }
    else
    {
      refPolygon = &polygon1;
      refNormal = &normal1;
      incPolygon = &polygon2;
      incNormal = &normal2;

      refAdjPlanes = &adjPlanes1;
    }

    Plane refPlane = Plane(-(*refNormal), -Vector3::Dot(-(*refNormal), refPolygon->front()));

    // Determine largest penetration
    float penetrationOffset = -FLT_MAX;
    for (auto it = refPolygon->begin(); it != refPolygon->end(); ++it)
    {
      float pOffset = Vector3::Dot(*it, m_BestColData._normal);
      if (pOffset > penetrationOffset)
        penetrationOffset = pOffset;
    }

    // Clip adjacent contact points
    SutherlandHodgmanClipping(*incPolygon, refAdjPlanes->size(), &(*refAdjPlanes)[0], incPolygon, false);

    // Clip above contact points
    SutherlandHodgmanClipping(*incPolygon, 1, &refPlane, incPolygon, true);

    // Process remaining contact points
    if (!incPolygon->empty())
    {
      Vector3 startPoint = incPolygon->back();

      for (auto it = incPolygon->begin(); it != incPolygon->end(); ++it)
      {
        float contactPenetration;
        Vector3 globalOnA, globalOnB;

        if (flipped)
        {
          contactPenetration = -Vector3::Dot(*it, m_BestColData._normal) + penetrationOffset;
          globalOnA = *it + (m_BestColData._normal * contactPenetration);
          globalOnB = *it;
        }
        else
        {
          contactPenetration = Vector3::Dot(*it, m_BestColData._normal) - penetrationOffset;
          globalOnA = *it;
          globalOnB = *it - (m_BestColData._normal * contactPenetration);
        }

        if (contactPenetration < 0.0f)
          out_manifold->AddContact(globalOnA, globalOnB, m_BestColData._normal, contactPenetration);

        startPoint = *it;
      }
    }
  }
}

Vector3 CollisionDetectionSAT::PlaneEdgeIntersection(const Plane &plane, const Vector3 &start, const Vector3 &end) const
{
  Vector3 ab = end - start;

  // Check that the edge and plane are not parallel and thus never intersect
  // We do this by projecting ab along the plane
  float ab_p = Vector3::Dot(plane.GetNormal(), ab);
  if (fabs(ab_p) > 1e-6f)
  {
    // Generate a random point on the plane (any point on the plane will
    // suffice)
    Vector3 p_co = plane.GetNormal() * (-plane.GetDistance());

    // Work out the edge factor to scale edge by
    // e.g. how far along the edge to traverse before it meets the plane.
    //      This is computed by: -proj<plane_nrml>(edge_start - planar_point) /
    //      proj<plane_nrml>(edge_start - edge_end)
    float fac = -Vector3::Dot(plane.GetNormal(), start - p_co) / ab_p;

    // Return point on edge
    return start + ab * fac;
  }

  return start;
}

void CollisionDetectionSAT::SutherlandHodgmanClipping(const std::list<Vector3> &input_polygon, int num_clip_planes,
                                                      const Plane *clip_planes, std::list<Vector3> *out_polygon,
                                                      bool removePoints) const
{
  if (!out_polygon)
    return;

  // Create temporary list of vertices
  // - We will keep ping-pong'ing between
  //   the two lists updating them as we go.
  std::list<Vector3> ppPolygon1, ppPolygon2;
  std::list<Vector3> *input = &ppPolygon1, *output = &ppPolygon2;

  *output = input_polygon;

  // Iterate over each clip_plane provided
  for (int i = 0; i < num_clip_planes; ++i)
  {
    // If we every single point on our shape has already been removed, just exit
    if (output->empty())
      break;

    const Plane &plane = clip_planes[i];

    // Swap input/output polygons, and clear output list for us to generate afresh
    std::swap(input, output);
    output->clear();

    // Loop through each edge of the polygon (see line_loop from gfx) and clips
    // that edge against the plane.
    Vector3 startPoint = input->back();
    for (const Vector3 &endPoint : *input)
    {
      bool startInPlane = plane.PointInPlane(startPoint);
      bool endInPlane = plane.PointInPlane(endPoint);

      // If it's the final pass, just remove all points outside the reference
      // plane
      if (removePoints)
      {
        if (endInPlane)
          output->push_back(endPoint);
      }
      else
      {
        // if entire edge is within the clipping plane, keep it as it is
        if (startInPlane && endInPlane)
          output->push_back(endPoint);

        // if edge interesects the clipping plane, cut the edge along clip plane
        else if (startInPlane && !endInPlane)
        {
          output->push_back(PlaneEdgeIntersection(plane, startPoint, endPoint));
        }
        else if (!startInPlane && endInPlane)
        {
          output->push_back(PlaneEdgeIntersection(plane, endPoint, startPoint));
          output->push_back(endPoint);
        }
      }
      //..otherwise the edge is entirely outside the clipping plane and should
      // be removed

      startPoint = endPoint;
    }
  }

  *out_polygon = *output;
}
