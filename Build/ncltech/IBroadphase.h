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

/**
 * @class IBroadphase
 * @author Dan Nixon
 * @brief Interface for broadphase collision detection culling.
 */
class IBroadphase
{
public:
  /**
   * @brief Obtains a list of potential collision pairs.
   * @param objects All objects in scene
   * @param collisionPairs Possible collision pairs found
   */
  virtual void FindPotentialCollisionPairs(std::vector<PhysicsObject *> &objects, std::vector<CollisionPair> &collisionPairs) = 0;

  /**
   * @brief Perform visual debugging of culling method.
   */
  virtual void DebugDraw() = 0;
};
