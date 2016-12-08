#include "OctreeBroadphase.h"

OctreeBroadphase::OctreeBroadphase(size_t maxObjectsPerPartition, size_t maxPartitionDepth)
    : m_maxObjectsPerPartition(maxObjectsPerPartition)
    , m_maxPartitionDepth(maxPartitionDepth)
{
}

OctreeBroadphase::~OctreeBroadphase()
{
}

void OctreeBroadphase::FindPotentialCollisionPairs(std::vector<PhysicsObject *> &objects,
                                                   std::vector<CollisionPair> &collisionPairs)
{
  // TODO
}