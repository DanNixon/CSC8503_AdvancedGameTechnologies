#pragma once

#include <ncltech/StateMachine.h>

/**
 * @class MockStateMachine
 * @brief Mock state machine used for testing with MockState.
 * @author Dan Nixon
 * @see MockState
 */
class MockStateMachine : public StateMachine
{
public:
  /**
   * @copydoc StateMachine::operate
   */
  virtual void operate()
  {
    operatedStack.clear();
    StateMachine::Operate();
  }

public:
  int value;                   //!< Dummy value for triggering state transitions
  IStatePtrList entryStack;    //!< States that have had onEntry() called
  IStatePtrList exitStack;     //!< States that have had onExit() called
  IStatePtrList operatedStack; //!< States that have had onOperate() called
};