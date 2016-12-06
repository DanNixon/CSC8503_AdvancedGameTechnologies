#pragma once

#include "PathNode.h"

/**
 * @class PathEdge
 * @brief Represents a connection between two nodes.
 * @author Dan Nixon
 */
class PathEdge
{
public:
  PathEdge(PathNode *a, PathNode *b);
  virtual ~PathEdge();

  /**
   * @brief Gets the first node.
   * @return First node
   */
  inline PathNode *NodeA()
  {
    return m_nodeA;
  }

  /**
   * @brief Gets the second node.
   * @return Second node
   */
  inline PathNode *NodeB()
  {
    return m_nodeB;
  }

  PathNode *OtherNode(PathNode *node);

  /**
   * @brief Tests if this edge is traversable.
   * @return True if this edge can be traversed
   */
  inline bool Traversable() const
  {
    return m_traversable;
  }

  /**
   * @brief Gets the static cost of traversing this edge.
   * @return Static cost
   */
  virtual float StaticCost() const;

  /**
   * @brief Gets the weight coefficient of traversing this edge.
   * @return Edge weight
   */
  inline float Weight() const
  {
    return m_weight;
  }

  /**
   * @brief Gets the weighted cost of traversing this edge.
   * @return Weighted cost
   */
  inline float Cost() const
  {
    return StaticCost() * m_weight;
  }

  void SetTraversable(bool traversable);
  void SetWeight(float weight);

  /**
   * @brief Checks of an Edge is on a path.
   * @param path Path to check
   * @return True if edge is on the path
   *
   * Edges are on a path if both nodes are on the path and are a distance of 1 apart.
   */
  bool IsOnPath(const std::vector<PathNode *> &path)
  {
    auto aIt = std::find(path.begin(), path.end(), m_nodeA);
    auto bIt = std::find(path.begin(), path.end(), m_nodeB);
    return (aIt != path.end() && bIt != path.end() && std::abs(std::distance(aIt, bIt)) == 1);
  }

  bool operator==(const PathEdge &other) const;
  bool operator!=(const PathEdge &other) const;

private:
  PathNode *m_nodeA; //!< First node
  PathNode *m_nodeB; //!< Second node

protected:
  bool m_traversable; //!< Flag indicating if this edge can be traversed
  float m_weight;     //!< Weight coefficient of traversing this edge
};