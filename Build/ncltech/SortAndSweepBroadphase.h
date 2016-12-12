#pragma once

#include "IBroadphase.h"

#include <functional>
#include <nclgl/Vector3.h>

/**
 * @class SortAndSweepBroadphase
 * @author Dan Nixon
 * @brief Broadphase collision pair culling using the sort and sweep method.
 */
class SortAndSweepBroadphase : public IBroadphase
{
public:
  SortAndSweepBroadphase(const Vector3 &axis = Vector3(1.0f, 0.0f, 0.0f));
  virtual ~SortAndSweepBroadphase();

  /**
   * @brief Gets the axis operated along.
   * @return Axis
   */
  inline Vector3 Axis() const
  {
    return m_axis;
  }

  void SetAxis(const Vector3 &axis);

  virtual void FindPotentialCollisionPairs(std::vector<PhysicsObject *> &objects, std::vector<CollisionPair> &collisionPairs);
  virtual void DebugDraw();

protected:
  Vector3 m_axis;  //!< Axis along which testing is performed
  int m_axisIndex; //!< Index of axis along which testing is performed

  std::function<bool(PhysicsObject *, PhysicsObject *)> m_sortComparator; //!< Comparator used for sorting along axis
};
