#include "Node.h"

#include <string>
#include <vector>

#include "Edge.h"

  /**
   * @brief Creates a new node.
   * @param id String ID
   * @param position Position of the node in 3 dimensions
   */
  Node::Node(const std::string &id, const Vector3 &position)
      : m_id(id)
      , m_position(position)
  {
  }

  /**
   * Copy constructor.
   * @param n Other node
   */
  Node::Node(const Node &n)
      : m_id(n.m_id)
      , m_position(n.m_position)
  {
  }

  Node::~Node()
  {
  }

  /**
   * @brief Outputs a Node to a stream.
   * @param stream Stream to output to
   * @param n Node to output
   * @return Reference to stream
   */
  std::ostream &operator<<(std::ostream &stream, const Node &n)
  {
    stream << n.m_id << ":" << n.m_position;
    return stream;
  }