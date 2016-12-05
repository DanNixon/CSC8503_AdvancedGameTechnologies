#include "SortAndSweepBroadphase.h"

#include <algorithm>

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

  // Generate comparator
  // TODO
  m_sortComparator = [](PhysicsObject *a, PhysicsObject *b) { return a->GetAABB().GetLower().x < b->GetAABB().GetLower().x; };
}

void SortAndSweepBroadphase::FindPotentialCollisionPairs(std::vector<PhysicsObject *> &objects,
                                                         std::vector<CollisionPair> &collisionPairs)
{
  // Sort entities along axis
  std::sort(objects.begin(), objects.end(), m_sortComparator);

  for (auto it = objects.begin(); it != objects.end(); ++it)
  {
    float thisBoxRight = (*it)->GetAABB().GetUpper().x; // TODO

    for (auto iit = it + 1; iit != objects.end(); ++iit)
    {
      // Skip pairs of two at rest objects
      if ((*it)->IsAtRest() && (*iit)->IsAtRest())
        continue;

      float testBoxLeft = (*iit)->GetAABB().GetLower().x; // TODO

      // Test for overlap between the axis values of the bounding boxes
      if (testBoxLeft < thisBoxRight)
      {
        CollisionPair cp;
        cp.pObjectA = *it;
        cp.pObjectB = *iit;

        collisionPairs.push_back(cp);
      }
    }
  }
}