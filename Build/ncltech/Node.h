#pragma once

#include <vector>

#include <nclgl/Vector3.h>

  class Edge;

  /**
   * @class Node
   * @brief Represents a node in a map.
   * @author Dan Nixon
   */
  class Node
  {
  public:
    Node(const std::string &id, const Vector3 &position = Vector3());
    Node(const Node &n);
    virtual ~Node();

    /**
     * @brief Gets the ID of the node.
     * @return String ID
     */
    inline std::string id() const
    {
      return m_id;
    }

    /**
     * @brief Gets the position of this node.
     * @return Node position
     */
    inline Vector3 position() const
    {
      return m_position;
    }

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
    inline Edge *edge(size_t i)
    {
      return m_connections[i];
    }

    /**
     * @brief Checks if node is on a given list.
     * @param list List to check
     * @return True if node was found on list
     */
    bool IsOnList(const std::vector<Node *> &list)
    {
      return std::find(list.begin(), list.end(), this) != list.end();
    }

    /**
     * @brief Gets the heuristic cost of traversing the path between this node and another.
     * @param other Other node
     * @return Heuristic cost
     */
    virtual float h(const Node &other) const = 0;

    friend std::ostream &operator<<(std::ostream &stream, const Node &n);

  private:
    friend class Edge;

    std::string m_id;                  //!< String ID of the node
    std::vector<Edge *> m_connections; //!< Edges of this node
    Vector3 m_position; //!< Posiion of this node
  };