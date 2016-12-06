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
  // Determine axis
  m_axis = axis;
  m_axis.Normalise();

  if (abs(m_axis.x) > 0.9f)
    m_axisIndex = 0;
  else if (abs(m_axis.y) > 0.9f)
    m_axisIndex = 1;
  else if (abs(m_axis.z) > 0.9f)
    m_axisIndex = 2;

  // Generate comparator
  m_sortComparator = [this](PhysicsObject *a, PhysicsObject *b) {
    return a->GetWorldSpaceAABB().GetLower()[this->m_axisIndex] < b->GetWorldSpaceAABB().GetLower()[this->m_axisIndex];
  };
}

void SortAndSweepBroadphase::FindPotentialCollisionPairs(std::vector<PhysicsObject *> &objects,
                                                         std::vector<CollisionPair> &collisionPairs)
{
  // Sort entities along axis
  std::sort(objects.begin(), objects.end(), m_sortComparator);

  for (auto it = objects.begin(); it != objects.end(); ++it)
  {
    float thisBoxRight = (*it)->GetWorldSpaceAABB().GetUpper()[m_axisIndex];

    for (auto iit = it + 1; iit != objects.end(); ++iit)
    {
      // Skip pairs of two at rest objects
      if ((*it)->IsAtRest() && (*iit)->IsAtRest())
        continue;

      float testBoxLeft = (*iit)->GetWorldSpaceAABB().GetLower()[m_axisIndex];

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