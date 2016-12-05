#pragma once

#include "FunctionalState.h"

/**
 * @class CompletableActionState
 * @brief Used for a state that represents an action that can be completed.
 * @author Dan Nixon
 */
class CompletableActionState : public FunctionalState
{
public:
  /**
   * @copydoc FunctionalState::FunctionalState
   */
  CompletableActionState(const std::string &name, IState *parent, StateMachine *machine)
      : FunctionalState(name, parent, machine)
      , m_completed(false)
  {
  }

  /**
   * @brief Check if the action is completed.
   * @return True if complete
   */
  bool completed() const
  {
    return m_completed;
  }

  /**
   * @brief Mark the action as complete.
   */
  void markAsComplete()
  {
    m_completed = true;
  }

protected:
  /**
   * @copydoc FunctionalState::onEntry
   */
  virtual void onEntry(IState *last)
  {
    FunctionalState::onEntry(last);

    // Mark as not completed on state entry
    m_completed = false;
  }

protected:
  bool m_completed; //!< Flag indicating completion
};