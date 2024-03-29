#include "StateContainer.h"

#include <algorithm>

#include "State.h"
#include "Utility.h"

StateContainer::StateContainer()
    : m_active(nullptr)
{
}

StateContainer::~StateContainer()
{
  for (auto it = m_children.begin(); it != m_children.end(); ++it)
    delete *it;
}

/**
 * @brief Finds a state in this sub tree given its path.
 * @name Path to state
 * @return State branch
 */
StatePtrList StateContainer::FindState(const std::string &name, char delim)
{
  StatePtrList branch;
  std::vector<std::string> names = Utility::Split(name, delim);
  std::reverse(names.begin(), names.end());
  FindStateImpl(names, branch);
  return branch;
}

/**
 * @brief Performs recursive search for a state within this sub tree.
 * @param names Reference to vector of names of states in branch in reverse order
 * @param branch Reference to the vector containing branch states
 * @return True if search was succesfull
 */
bool StateContainer::FindStateImpl(std::vector<std::string> &names, StatePtrList &branch)
{
  // Find child by name
  const std::string name(names.back());
  auto it = std::find_if(m_children.begin(), m_children.end(), [name](State *i) { return i->Name() == name; });

  // Fail if child not found
  if (it == m_children.end())
    return false;

  // Remove found name and add found state
  branch.push_back(*it);
  names.pop_back();

  // If found sttae has children and there are still names to find then search further
  if ((*it)->HasChildren() && !names.empty())
    return (*it)->FindStateImpl(names, branch);
  else
    return true;
}