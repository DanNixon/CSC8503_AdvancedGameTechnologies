#pragma once

#include <vector>

#include "PathNode.h"

/**
 * @class QueueablePathNode
 * @brief Wrapper around a Node for use in AStar.
 * @author Dan Nixon
 *
 * Contains comparators for priority queue and a pointer to a parent node.
 */
class QueueablePathNode
{
public:
  /**
   * @brief Creates a new wrapper.
   * @param n Wrapped node
   */
  QueueablePathNode(PathNode *n)
      : node(n)
      , parent(nullptr)
      , fScore(std::numeric_limits<float>::max())
      , gScore(std::numeric_limits<float>::max())
  {
  }

  virtual ~QueueablePathNode()
  {
  }

  /**
   * @brief Checks if a node is on a given list.
   * @param list List to check
   * @return True if node was found on list
   */
  bool IsOnList(const std::vector<QueueablePathNode *> &list)
  {
    return std::find_if(list.begin(), list.end(), [this](QueueablePathNode *n) { return n->node == (PathNode *)this; }) !=
           list.end();
  }

  /**
   * @brief Tests for difference between F values of this node and another.
   * @param other Node wrapper being compared with
   * @return True if this node has an F value greater than than that of
   *         other.
   */
  bool operator>(const QueueablePathNode &other) const
  {
    return this->fScore > other.fScore;
  }

  /**
   * @brief Tests for difference between F values of this node and another.
   * @param other Node wrapper being compared with
   * @return True if this node has an F value greater than of equal to that
   *         of other.
   */
  bool operator>=(const QueueablePathNode &other) const
  {
    return this->fScore >= other.fScore;
  }

  /**
   * @brief Tests for difference between F values of this node and another.
   * @param other Node wrapper being compared with
   * @return True if this node has an F value less than that of other.
   */
  bool operator<(const QueueablePathNode &other) const
  {
    return this->fScore < other.fScore;
  }

  /**
   * @brief Tests for difference between F values of this node and another.
   * @param other Node wrapper being compared with
   * @return True if this node has an F value less than or equal to that of
   *         other.
   */
  bool operator<=(const QueueablePathNode &other) const
  {
    return this->fScore <= other.fScore;
  }

public:
  PathNode *node;            //!< Wrapped Node
  QueueablePathNode *parent; //!< Parent Node in path
  float fScore;              //!< F score of wrapped node
  float gScore;              //!< G score of wrapped node
};
