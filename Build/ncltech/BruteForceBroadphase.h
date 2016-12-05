#pragma once

#include "IBroadphase.h"

class BruteForceBroadphase : public IBroadphase
{
public:
  BruteForceBroadphase();
  virtual ~BruteForceBroadphase();

  virtual void FindPotentialCollisionPairs(const std::vector<PhysicsObject *> & objects, std::vector<CollisionPair> & collisionPairs);
};