#pragma once

#include "StateContainer.h"

#include <functional>
#include <iostream>
#include <string>
#include <vector>

class StateMachine;

/**
 * @typedef IStatePtrList
 * @brief List of pointers to IState instances.
 */
typedef std::vector<IState *> IStatePtrList;

/**
 * @class IState
 * @brief Represents a state in a state machine.
 */
class IState : public StateContainer
{
public:
  typedef std::function<IState *()> TransferFromTest;
  typedef std::function<bool()> TransferToTest;
  typedef std::function<void(IState *)> OnEntryBehaviour;
  typedef std::function<void(IState *)> OnExitBehaviour;
  typedef std::function<void()> OnOperateBehaviour;

public:
  static IState *ClosestCommonAncestor(IState *a, IState *b);

public:
  IState(const std::string &name, IState *parent, StateMachine *machine);
  virtual ~IState();

  /**
   * @brief Gets the name of this IState.
   * @return IState name
   */
  inline std::string name() const
  {
    return m_name;
  }

  /**
   * @brief Returns the parent state of this state.
   * @return Parent state
   */
  inline IState *parent() const
  {
    return m_parent;
  }

  /**
   * @brief Checks if this IState is active.
   * @return True if this IState is active
   */
  inline bool isActive() const
  {
    return m_parent->activeChild() == this;
  }

  void AddTransferFromTest(TransferFromTest test);
  void AddTransferToTest(TransferToTest test);
  void AddOnEntryBehaviour(OnEntryBehaviour behaviour);
  void AddOnExitBehaviour(OnExitBehaviour behaviour);
  void AddOnOperateBehaviour(OnOperateBehaviour behaviour);

  IStatePtrList branch(bool reverse = false);

  void setActivation(bool active, IState *terminateAt = nullptr, IState *delta = nullptr);

protected:
  friend class StateMachine;

  IState *testTransferFrom() const;
  bool testTransferTo() const;

  void onEntry(IState *last);
  void onExit(IState *next);
  void onOperate();

protected:
  const std::string m_name; //!< Name of this state
  StateMachine *m_machine;  //!< State machine that holds this state
  IState *m_parent;         //!< Parent state

  std::vector<TransferFromTest> m_transferFromTests;
  std::vector<TransferToTest> m_transferToTests;
  std::vector<OnEntryBehaviour> m_onEntryBehaviours;
  std::vector<OnExitBehaviour> m_onExitBehaviours;
  std::vector<OnOperateBehaviour> m_onOperateBehaviours;
};