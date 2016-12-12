#include "OctreeBroadphase.h"

OctreeBroadphase::OctreeBroadphase(size_t maxObjectsPerPartition, size_t maxPartitionDepth, IBroadphase *secondaryBroadphase)
    : m_maxObjectsPerPartition(maxObjectsPerPartition)
    , m_maxPartitionDepth(maxPartitionDepth)
    , m_secondaryBroadphase(secondaryBroadphase)
{
}

OctreeBroadphase::~OctreeBroadphase()
{
  delete m_secondaryBroadphase;
}

void OctreeBroadphase::FindPotentialCollisionPairs(std::vector<PhysicsObject *> &objects,
                                                   std::vector<CollisionPair> &collisionPairs)
{
  // Delete old data
  delete m_world;
  m_leafDivisions.clear();

  // Init root world space
  m_world = new WorldDivision();
  for (auto it = objects.begin(); it != objects.end(); ++it)
  {
    PhysicsObject * o = *it;
    
    m_world->box.ExpandToFit(o->GetWorldSpaceAABB());
    m_world->objects.push_back(o);
  }

  // Recursively divide world
  DivideWorld(m_world, 0);

  // Add collision pairs in leaf world divisions
  for (auto it = m_leafDivisions.begin(); it != m_leafDivisions.end(); ++it)
    m_secondaryBroadphase->FindPotentialCollisionPairs((*it)->objects, collisionPairs);
}

void OctreeBroadphase::DebugDraw()
{
  DebugDrawWorldDivision(m_world);
}

void OctreeBroadphase::DivideWorld(WorldDivision *division, size_t iteration)
{
  // Exit conditions (partition depth limit or target object count reached)
  if (iteration > m_maxPartitionDepth || division->objects.size() <= m_maxObjectsPerPartition)
  {
    m_leafDivisions.push_back(division);
    return;
  }

  Vector3 divisionPoints[] = {division->box.Lower(), division->box.Centre(), division->box.Upper()};

  // clang-format off
  static const size_t NUM_DIVISIONS = 8;
  static const size_t DIVISION_POINT_INDICES[NUM_DIVISIONS][6] = {
    {0, 0, 0, 1, 1, 1},
    {1, 0, 0, 2, 1, 1},
    {0, 1, 0, 1, 2, 1},
    {1, 1, 0, 2, 2, 1},
    {0, 0, 1, 1, 1, 2},
    {1, 0, 1, 2, 1, 2},
    {0, 1, 1, 1, 2, 2},
    {1, 1, 1, 2, 2, 2}
  };
  // clang-format on

  // Create new world divisions
  for (size_t i = 0; i < NUM_DIVISIONS; i++)
  {
    WorldDivision * newDivision = new WorldDivision();

    // Set bounds
    // clang-format off
    Vector3 lower(divisionPoints[DIVISION_POINT_INDICES[i][0]].x,
                  divisionPoints[DIVISION_POINT_INDICES[i][1]].y,
                  divisionPoints[DIVISION_POINT_INDICES[i][2]].z);
    Vector3 upper(divisionPoints[DIVISION_POINT_INDICES[i][3]].x,
                  divisionPoints[DIVISION_POINT_INDICES[i][4]].y,
                  divisionPoints[DIVISION_POINT_INDICES[i][5]].z);
    // clang-format on

    newDivision->box = BoundingBox(lower, upper);

    // Add objects inside division
    for (auto it = division->objects.begin(); it != division->objects.end(); ++it)
    {
      if (newDivision->box.Intersects((*it)->GetWorldSpaceAABB()))
        newDivision->objects.push_back(*it);
    }

    // Add to parent division
    division->subdivisions.push_back(newDivision);

    // Do further subdivisioning
    DivideWorld(newDivision, iteration + 1);
  }
}

void OctreeBroadphase::DebugDrawWorldDivision(WorldDivision *division)
{
  // Draw bounding box
  division->box.DebugDraw(Matrix4(), Vector4(1.0f, 0.8f, 0.8f, 0.1f), Vector4(1.0f, 0.5f, 0.8f, 1.0f));

  // Draw sub divisions
  for (auto it = division->subdivisions.begin(); it != division->subdivisions.end(); ++it)
    DebugDrawWorldDivision(*it);
}
