#include "PathNode.h"

#include <string>
#include <vector>

#include "PathEdge.h"

/**
 * @brief Creates a new node.
 */
PathNode::PathNode(const Vector3 &position)
    : PhysicsObject()
{
  SetPosition(position);
}

PathNode::~PathNode()
{
}

/**
 * @brief Checks if node is on a given list.
 * @param list List to check
 * @return True if node was found on list
 */
bool PathNode::IsOnList(const std::vector<PathNode *> &list)
{
  return std::find(list.begin(), list.end(), this) != list.end();
}

/**
 * @brief Gets the heuristic cost of traversing the path between this node and another.
 * @param other Other node
 * @return Heuristic cost
 */
float PathNode::h(const PathNode &other) const
{
  return (GetWorldSpaceTransform().GetPositionVector() - other.GetWorldSpaceTransform().GetPositionVector()).Length();
}
