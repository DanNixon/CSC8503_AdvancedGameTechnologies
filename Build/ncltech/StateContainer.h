#pragma once

#include <string>
#include <vector>

class State;

/**
 * @class StateContainer
 * @brief Abstract class for classes that contain a set of states.
 * @author Dan Nixon
 */
class StateContainer
{
public:
  StateContainer();
  virtual ~StateContainer();

  std::vector<State *> FindState(const std::string &path, char delim = '/');

  /**
   * @brief Checks if this node has child states.
   * @return True if child states exist
   */
  inline bool HasChildren() const
  {
    return m_children.size() > 0;
  }

  /**
   * @brief Adds a child state.
   * @param child State to add
   */
  inline void AddChild(State *child)
  {
    m_children.push_back(child);
  }

  /**
   * @brief Gets the active child state of this node.
   * @return Active child state
   */
  inline State *ActiveChild() const
  {
    return m_active;
  }

  /**
   * @brief Checks if a given child state is active.
   * @param state State to test
   * @return True if state is active
   */
  inline bool IsChildActive(const State *state) const
  {
    return m_active == state;
  }

  /**
   * @brief Gets a reference to the vector containing child states.
   * @return Reference to children vector
   */
  inline std::vector<State *> Children()
  {
    return m_children;
  }

private:
  bool FindStateImpl(std::vector<std::string> &names, std::vector<State *> &branch);

protected:
  State *m_active;                 //!< Active state
  std::vector<State *> m_children; //!< Child states
};