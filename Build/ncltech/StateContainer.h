#pragma once

#include <string>
#include <vector>

class IState;

/**
 * @class StateContainer
 * @brief Abstract class for classes that contain a set of states.
 * @author Dan Nixon
 */
class StateContainer
{
public:
  static std::vector<std::string> StateContainer::Split(const std::string &str, char delim = '/');

public:
  StateContainer();
  virtual ~StateContainer();

  std::vector<IState *> findState(const std::string &path, char delim = '/');

  /**
   * @brief Checks if this node has child states.
   * @return True if child states exist
   */
  inline bool hasChildren() const
  {
    return m_children.size() > 0;
  }

  /**
   * @brief Adds a child state.
   * @param child State to add
   */
  inline void addChild(IState *child)
  {
    m_children.push_back(child);
  }

  /**
   * @brief Gets the active child state of this node.
   * @return Active child state
   */
  inline IState *activeChild() const
  {
    return m_active;
  }

  /**
   * @brief Checks if a given child state is active.
   * @param state State to test
   * @return True if state is active
   */
  inline bool isChildActive(const IState *state) const
  {
    return m_active == state;
  }

  /**
   * @brief Gets a reference to the vector containing child states.
   * @return Reference to children vector
   */
  inline std::vector<IState *> children()
  {
    return m_children;
  }

private:
  bool findStateImpl(std::vector<std::string> &names, std::vector<IState *> &branch);

protected:
  IState *m_active;                 //!< Active state
  std::vector<IState *> m_children; //!< Child states
};