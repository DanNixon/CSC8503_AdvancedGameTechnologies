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
   * @brief Sets the state returned to when this state machine is reset.
   * @param defaultState Pointer to default state
   */
  void SetDefaultState(IState *defaultState)
  {
    m_defaultState = defaultState;
  }

  void Reset();

  /**
   * @brief Performs updates of the state machine by checking for transition conditions then performing operations over all active
   *        states.
   * @param dt Time since the last call to this function
   * @return True if a state change took place
   * @see StateMachine:Transfer()
   * @see StateMachine:Operate()
   */
  inline bool Update(float dt = 0.0f)
  {
    bool stateChange = Transfer();
    Operate(dt);
    return stateChange;
  }

  virtual bool Transfer();
  virtual void Operate(float dt = 0.0f);

private:
  IState m_root;          //!< Root state
  IState *m_defaultState; //!< Default state
};