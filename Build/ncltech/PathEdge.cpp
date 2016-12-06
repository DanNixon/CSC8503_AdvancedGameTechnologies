#include "PathEdge.h"

#include <string>

/**
 * @brief Creates a new edge.
 * @param a First node
 * @param b Second node
 */
PathEdge::PathEdge(PathNode *a, PathNode *b)
    : m_nodeA(a)
    , m_nodeB(b)
    , m_traversable(true)
    , m_weight(1.0f)
{
  // Associate this edge with the nodes
  m_nodeA->m_connections.push_back(this);
  m_nodeB->m_connections.push_back(this);
}

PathEdge::~PathEdge()
{
}

/**
 * @brief Gets the node an the edge that is not a given node.
 * @param node A node on the edge
 * @return Pointer to the other node on the edge, nullptr if node is not on
 *         this edge
 */
PathNode *PathEdge::OtherNode(PathNode *node)
{
  PathNode *retVal = nullptr;

  if (node == m_nodeA)
    retVal = m_nodeB;
  else if (node == m_nodeB)
    retVal = m_nodeA;

  return retVal;
}

float PathEdge::StaticCost() const
{
  return (m_nodeA->GetWorldSpaceTransform().GetPositionVector() - m_nodeB->GetWorldSpaceTransform().GetPositionVector()).Length();
}

/**
 * @brief Sets if this edge can be traversed.
 * @param traversable True if this edge can be traversed
 */
void PathEdge::SetTraversable(bool traversable)
{
  m_traversable = traversable;
}

/**
 * @brief Sets the traversal weighting of this edge.
 * @param weight Edge weight
 */
void PathEdge::SetWeight(float weight)
{
  m_weight = weight;
}

/**
 * @brief Tests for equality between this edge and another.
 * @param other Other edge
 * @return True if both edges exist between the same two nodes
 *
 * Order of nodes is ignored.
 */
bool PathEdge::operator==(const PathEdge &other) const
{
  return (m_nodeA == other.m_nodeA && m_nodeB == other.m_nodeB) || (m_nodeA == other.m_nodeB && m_nodeB == other.m_nodeA);
}

/**
 * @brief Tests for inequality between this edge and another.
 * @param other Other edge
 * @return True if both edges exist between two different nodes
 *
 * Order of nodes is ignored.
 */
bool PathEdge::operator!=(const PathEdge &other) const
{
  return !this->operator==(other);
}