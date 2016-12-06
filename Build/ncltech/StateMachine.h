#pragma once

#include "StateContainer.h"

#include <vector>

#include "IState.h"

/**
 * @class StateMachine
 * @brief Represents a state machine
 * @author Dan Nixon
 */
class StateMachine
{
public:
  static std::string BranchToString(const IStatePtrList &branch, char delim = '/');

public:
  StateMachine();
  virtual ~StateMachine();

  IStatePtrList ActiveStateBranch();

  /**
   * @brief Gets the root state for the state machine.
   * @return Root state
   */
  IState *RootState()
  {
    return &m_root;
  }

  /**
   * @brief Performs updates of the state machine by checking for transition
   *        conditions then performing operations over all active states.
   * @return True if a state change took place
   * @see StateMachine:transfer()
   * @see StateMachine:operate()
   */
  inline bool Update()
  {
    bool stateChange = Transfer();
    Operate();
    return stateChange;
  }

  virtual bool Transfer();
  virtual void Operate();

private:
  IState m_root; //!< Root state
};