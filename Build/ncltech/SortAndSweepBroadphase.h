#pragma once

#include "IBroadphase.h"
#include <functional>
#include <nclgl/Vector3.h>

class SortAndSweepBroadphase : public IBroadphase
{
public:
  SortAndSweepBroadphase(const Vector3 &axis = Vector3(1.0f, 0.0f, 0.0f));
  virtual ~SortAndSweepBroadphase();

  inline Vector3 Axis() const
  {
    return m_axis;
  }

  void SetAxis(const Vector3 &axis);

  virtual void FindPotentialCollisionPairs(std::vector<PhysicsObject *> &objects, std::vector<CollisionPair> &collisionPairs);

protected:
  Vector3 m_axis;                                                         //!< Axis along which testing is performed
  std::function<bool(PhysicsObject *, PhysicsObject *)> m_sortComparator; //!< Comparator used for sorting along axis
};