#pragma once

#include "IBroadphase.h"

/**
 * @class BruteForceBroadphase
 * @author Dan Nixon
 * @brief Broadphase stage that finds all possible collision pairs between all objects in scene.
 *
 * Essentially equivalent of no broadphase.
 */
class BruteForceBroadphase : public IBroadphase
{
public:
  BruteForceBroadphase();
  virtual ~BruteForceBroadphase();

  virtual void FindPotentialCollisionPairs(std::vector<PhysicsObject *> &objects, std::vector<CollisionPair> &collisionPairs);
  virtual void DebugDraw();
};
