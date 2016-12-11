#pragma once

#include "IBroadphase.h"

class OctreeBroadphase : public IBroadphase
{
public:
  OctreeBroadphase(size_t maxObjectsPerPartition, size_t maxPartitionDepth);
  virtual ~OctreeBroadphase();

  virtual void FindPotentialCollisionPairs(std::vector<PhysicsObject *> &objects, std::vector<CollisionPair> &collisionPairs);

  virtual void DebugDraw();

protected:
  size_t m_maxObjectsPerPartition;
  size_t m_maxPartitionDepth;
};