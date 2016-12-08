#include "StateContainer.h"

#include <algorithm>
#include <sstream>

#include "State.h"

/**
* @brief Splits a string by a delimiter.
* @param str String to split
* @param delim Delimiter
* @return Vector of substrings
*/
std::vector<std::string> StateContainer::Split(const std::string &str, char delim)
{
  std::vector<std::string> retVal;
  std::stringstream ss(str);
  std::string item;

  while (std::getline(ss, item, delim))
  {
    if (!item.empty())
      retVal.push_back(item);
  }

  return retVal;
}

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
IStatePtrList StateContainer::FindState(const std::string &name, char delim)
{
  IStatePtrList branch;
  std::vector<std::string> names = Split(name, delim);
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
bool StateContainer::FindStateImpl(std::vector<std::string> &names, IStatePtrList &branch)
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