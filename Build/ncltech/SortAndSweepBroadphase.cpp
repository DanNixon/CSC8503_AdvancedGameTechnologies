#include "SortAndSweepBroadphase.h"

SortAndSweepBroadphase::SortAndSweepBroadphase(const Vector3 &axis)
    : IBroadphase()
{
  SetAxis(axis);
}

SortAndSweepBroadphase::~SortAndSweepBroadphase()
{
}

void SortAndSweepBroadphase::SetAxis(const Vector3 &axis)
{
  m_axis = axis;
  m_axis.Normalise();
}

void SortAndSweepBroadphase::FindPotentialCollisionPairs(const std::vector<PhysicsObject *> &objects,
                                                         std::vector<CollisionPair> &collisionPairs)
{
// TODO

#if 0
  // Sort entities along x-axis
  std::sort(m_entities.begin(), m_entities.end(),
    [](Entity *a, Entity *b) { return a->boundingBox().lowerLeft()[0] < b->boundingBox().lowerLeft()[0]; });

  std::vector<InterfaceDef> interfaces;

  // Create a list of possible interfaces (broadphase)
  for (EntityPtrListIter it = m_entities.begin(); it != m_entities.end(); ++it)
  {
    // Ignore entities with no collide
    if (!(*it)->collides())
      continue;

    bool thisStationary = (*it)->stationary();
    float thisBoxRight = (*it)->boundingBox().upperRight()[0];

    for (auto iit = it + 1; iit != m_entities.end(); ++iit)
    {
      // Two stationary (fixed) entities can never collide
      if (thisStationary && (*iit)->stationary())
        continue;

      float testBoxLeft = (*iit)->boundingBox().lowerLeft()[0];

      // Test for overlap between the x-axis values of the bounding boxes
      if (testBoxLeft < thisBoxRight)
        // If intersections exists then these entities could have collided
        interfaces.push_back(InterfaceDef(*it, *iit));
    }
  }
#endif
}