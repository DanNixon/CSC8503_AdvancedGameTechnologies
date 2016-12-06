#pragma once

#include <vector>

#include <nclgl/Vector3.h>

#include "PhysicsObject.h"

class PathEdge;

/**
 * @class PathNode
 * @brief Represents a node in a map.
 * @author Dan Nixon
 */
class PathNode : public PhysicsObject
{
public:
  PathNode(const Vector3 &position = Vector3());
  virtual ~PathNode();

  /**
   * @brief Gets the number of connections from this node.
   * @return Number of connections
   */
  inline size_t numConnections() const
  {
    return m_connections.size();
  }

  /**
   * @brief Gets an edge connecting this node to another.
   * @param i Index of edge to retrieve
   * @return Pointer to edge
   */
  inline PathEdge *edge(size_t i)
  {
    return m_connections[i];
  }

  bool IsOnList(const std::vector<PathNode *> &list);

  virtual float h(const PathNode &other) const;

private:
  friend class PathEdge;

  std::vector<PathEdge *> m_connections; //!< Edges of this node
};