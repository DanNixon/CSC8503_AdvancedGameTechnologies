#pragma once

#include "PhysicsObject.h"
#include <vector>

/**
 * @brief Forms the output of the broadphase collision detection
 */
struct CollisionPair
{
  PhysicsObject *pObjectA;
  PhysicsObject *pObjectB;
};

class IBroadphase
{
public:
  virtual void FindPotentialCollisionPairs(std::vector<PhysicsObject *> &objects,
                                           std::vector<CollisionPair> &collisionPairs) = 0;
};