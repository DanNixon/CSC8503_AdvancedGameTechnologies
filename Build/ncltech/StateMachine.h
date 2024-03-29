#pragma once

#include "StateContainer.h"

#include <vector>

#include "State.h"

/**
 * @class StateMachine
 * @brief Represents a state machine
 * @author Dan Nixon
 */
class StateMachine
{
public:
  static std::string BranchToString(const StatePtrList &branch, char delim = '/');

public:
  StateMachine();
  virtual ~StateMachine();

  StatePtrList ActiveStateBranch() const;

  /**
   * @brief Gets the root state for the state machine.
   * @return Root state
   */
  State *RootState()
  {
    return &m_root;
  }

  /**
   * @brief Sets the state returned to when this state machine is reset.
   * @param defaultState Pointer to default state
   */
  void SetDefaultState(State *defaultState)
  {
    m_defaultState = defaultState;
  }

  void Reset();
  void ActivateState(State *state, bool now = true);

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

protected:
  State m_root;               //!< Root state
  State *m_defaultState;      //!< Default state
  State *m_nextTransferState; //!< State to manually transfer to next iteration
};