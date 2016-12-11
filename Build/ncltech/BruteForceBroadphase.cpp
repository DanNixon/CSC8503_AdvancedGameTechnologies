#include "BruteForceBroadphase.h"

BruteForceBroadphase::BruteForceBroadphase()
    : IBroadphase()
{
}

BruteForceBroadphase::~BruteForceBroadphase()
{
}

void BruteForceBroadphase::FindPotentialCollisionPairs(std::vector<PhysicsObject *> &objects,
                                                       std::vector<CollisionPair> &collisionPairs)
{
  PhysicsObject *obj1, *obj2;
  if (objects.empty())
    return;

  for (size_t i = 0; i < objects.size() - 1; ++i)
  {
    for (size_t j = i + 1; j < objects.size(); ++j)
    {
      obj1 = objects[i];
      obj2 = objects[j];

      // Check they both have collision shapes and at least one is awake
      if (obj1->NumCollisionShapes() > 0 && obj2->NumCollisionShapes() > 0 && (obj1->IsAwake() || obj2->IsAwake()))
      {
        CollisionPair cp;
        cp.pObjectA = obj1;
        cp.pObjectB = obj2;

        collisionPairs.push_back(cp);
      }
    }
  }
}

void BruteForceBroadphase::DebugDraw()
{
  // Nothing to do here (all possible pairs are added)
}
