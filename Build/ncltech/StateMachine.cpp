#include "StateMachine.h"

#include <sstream>

/**
 * @brief Outputs a state branch as a string.
 * @param branch State branch
 * @param delim Delimiter
 * @return Delimited names of states in branch
 */
std::string StateMachine::BranchToString(const IStatePtrList &branch, char delim)
{
  std::stringstream str;
  for (auto it = branch.begin(); it != branch.end(); ++it)
  {
    if (it != branch.begin())
      str << delim;

    str << (*it)->m_name;
  }

  return str.str();
}

StateMachine::StateMachine()
    : m_root("", nullptr, this)
{
}

StateMachine::~StateMachine()
{
}

/**
 * @brief Gets the active branch of the state tree.
 * @return Branch of active states
 */
IStatePtrList StateMachine::ActiveStateBranch()
{
  IStatePtrList branch;

  IState *node = m_root.ActiveChild();
  while (node != nullptr)
  {
    branch.push_back(node);
    node = node->ActiveChild();
  }

  return branch;
}

/**
 * @brief Performs transfer checks for each level of the active state branch and switches states if needed.
 * @return True if a state change took place
 */
bool StateMachine::Transfer()
{
  IStatePtrList branch = ActiveStateBranch();
  IState *oldState = branch.back();
  bool stateChange = false;

  for (auto brIt = branch.begin(); brIt != branch.end(); ++brIt)
  {
    IState *transferState = (*brIt)->TestTransferFrom();

    if (transferState == nullptr)
    {
      IStatePtrList siblings = (*brIt)->Parent()->Children();
      for (auto sibIt = siblings.begin(); sibIt != siblings.end(); ++sibIt)
      {
        if ((*sibIt != *brIt) && (*sibIt)->TestTransferTo())
        {
          transferState = *sibIt;
          break;
        }
      }
    }

    if (transferState != nullptr)
    {
      stateChange = true;

      IState *commonAncestor = IState::ClosestCommonAncestor(oldState, transferState);
      oldState->SetActivation(false, commonAncestor, transferState);
      transferState->SetActivation(true, commonAncestor, oldState);

      break;
    }
  }

  return stateChange;
}

/**
 * @brief Performs operations associated with the currently active branch of the state tree.
 * @param dt Time since the last call to this function
 */
void StateMachine::Operate(float dt)
{
  IState *node = m_root.ActiveChild();
  while (node != nullptr)
  {
    node->m_timeInState += dt;
    node->OnOperate();
    node = node->ActiveChild();
  }
}