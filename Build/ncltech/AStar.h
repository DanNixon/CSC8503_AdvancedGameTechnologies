#pragma once

#include <map>
#include <vector>

#include "PathNode.h"
#include "PathNodePriorityQueue.h"
#include "QueueablePathNode.h"

/**
 * @class AStar
 * @brief Implementaton of the A* path finding algorithm.
 * @author Dan Nixon
 */
class AStar
{
public:
  AStar(const std::vector<PathNode *> &nodes);
  virtual ~AStar();

  void reset();
  bool findPath(PathNode *start, PathNode *end);

  /**
   * @brief Gets the open list.
   * @return Open list
   */
  inline PathNodePriorityQueue openList() const
  {
    return m_openList;
  }

  /**
   * @brief Gets the closed list.
   * @return Closed list
   */
  inline std::vector<QueueablePathNode *> closedList() const
  {
    return m_closedList;
  }

  /**
   * @brief Gets the computed path.
   * @return Path
   */
  inline std::vector<PathNode *> path() const
  {
    return m_path;
  }

  /**
   * @brief Gets the cost of the full path.
   * @return Path cost
   */
  inline float pathCost() const
  {
    return m_closedList.back()->gScore;
  }

private:
  std::map<PathNode *, QueueablePathNode *> m_nodeData;

  PathNodePriorityQueue m_openList;              //!< Open list
  std::vector<QueueablePathNode *> m_closedList; //!< Closed list

  std::vector<PathNode *> m_path; //!< Computed path
};