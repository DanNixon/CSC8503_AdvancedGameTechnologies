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
  // Reset state
  m_world.box.Reset();
  m_world.objects.clear();
  m_world.subdivisions.clear();

  // Add all objects to root world space
  m_world.objects.insert(m_world.objects.begin(), objects.begin(), objects.end());

  // Recursively divide world
  DivideWorld(m_world, 0);

  // Add collision pairs in leaf world divisions
  for (auto it = m_leafDivisions.begin(); it != m_leafDivisions.end(); ++it)
  {
    m_secondaryBroadphase->FindPotentialCollisionPairs((*it)->objects, collisionPairs);
  }
}

void OctreeBroadphase::DebugDraw()
{
  DebugDrawWorldDivision(m_world);
}

void OctreeBroadphase::DivideWorld(WorldDivision &division, size_t iteration)
{
  // Exit conditions (partition depth limit or target object count reached)
  if (iteration > m_maxPartitionDepth || division.objects.size() <= m_maxObjectsPerPartition)
  {
    m_leafDivisions.push_back(&division);
    return;
  }

  const Vector3 lower = division.box.Lower();
  const Vector3 centre = division.box.Centre();
  const Vector3 upper = division.box.Upper();

  // TODO
}

void OctreeBroadphase::DebugDrawWorldDivision(WorldDivision &division)
{
  // Draw bounding box
  division.box.DebugDraw(Matrix4(), Vector4(1.0f, 0.8f, 0.8f, 0.1f), Vector4(1.0f, 0.5f, 0.8f, 1.0f));

  // Draw sub divisions
  for (auto it = division.subdivisions.begin(); it != division.subdivisions.end(); ++it)
    DebugDrawWorldDivision(*it);
}
