#pragma once

#include "IBroadphase.h"

#include "BoundingBox.h"

class OctreeBroadphase : public IBroadphase
{
public:
  struct WorldDivision
  {
    ~WorldDivision()
    {
      for (auto it = subdivisions.begin(); it != subdivisions.end(); ++it)
        delete *it;
    }

    BoundingBox box;
    std::vector<PhysicsObject *> objects;
    std::vector<WorldDivision *> subdivisions;
  };

public:
  OctreeBroadphase(size_t maxObjectsPerPartition, size_t maxPartitionDepth, IBroadphase *secondaryBroadphase);
  virtual ~OctreeBroadphase();

  virtual void FindPotentialCollisionPairs(std::vector<PhysicsObject *> &objects, std::vector<CollisionPair> &collisionPairs);
  virtual void DebugDraw();

protected:
  void DivideWorld(WorldDivision *division, size_t iteration);
  void DebugDrawWorldDivision(WorldDivision *division);

protected:
  size_t m_maxObjectsPerPartition; //!< Maximum number of objects in a single world partition before subdivision
  size_t m_maxPartitionDepth;      //!< Maximum number of sub world partitions to recursively create

  IBroadphase *m_secondaryBroadphase; //!< Broadphase stage used to determine collision pairs within subdivisions

  WorldDivision *m_world; //!< Root world space
  std::vector<WorldDivision *> m_leafDivisions; //!< Subdivisions containing objects to generate collision pairs for
};