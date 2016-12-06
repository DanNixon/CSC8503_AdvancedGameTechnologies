#include "IState.h"

/**
 * @brief Finds the closest common ancestor of two states.
 * @param a First state
 * @param b Second state
 * @return Closest ancestor, nullptr if no common ancestor
 */
IState *IState::ClosestCommonAncestor(IState *a, IState *b)
{
  IStatePtrList branchA = a->branch();
  IStatePtrList branchB = b->branch();

  auto ia = branchA.begin();
  auto ib = branchB.begin();

  IState *commonAncestor = nullptr;

  // Taverse down common sections of each branch until the branches diverge
  while (*ia == *ib)
  {
    commonAncestor = *ia;

    ++ia;
    ++ib;

    // Exit when either branch terminates (i.e. either state is an ancestor of the other)
    if (ia == branchA.end() || ib == branchB.end())
      break;
  }

  return commonAncestor;
}

/**
 * @brief Create a new state.
 * @param name Name of this state
 * @param parent Parent state
 * @param machine The state machine to which this state belongs
 */
IState::IState(const std::string &name, IState *parent, StateMachine *machine)
    : m_name(name)
    , m_machine(machine)
    , m_parent(parent)
{
  if (parent)
    parent->m_children.push_back(this);
}

IState::~IState()
{
}

void IState::AddTransferFromTest(TransferFromTest test)
{
  if (test)
    m_transferFromTests.push_back(test);
}

void IState::AddTransferToTest(TransferToTest test)
{
  if (test)
    m_transferToTests.push_back(test);
}

void IState::AddOnEntryBehaviour(OnEntryBehaviour behaviour)
{
  if (behaviour)
    m_onEntryBehaviours.push_back(behaviour);
}

void IState::AddOnExitBehaviour(OnExitBehaviour behaviour)
{
  if (behaviour)
    m_onExitBehaviours.push_back(behaviour);
}

void IState::AddOnOperateBehaviour(OnOperateBehaviour behaviour)
{
  if (behaviour)
    m_onOperateBehaviours.push_back(behaviour);
}

/**
 * @brief Gets the branch of the state tree that leads to this node.
 * @param reverse If the branch order should be reversed
 * @return State tree branch
 */
IStatePtrList IState::branch(bool reverse)
{
  IStatePtrList branch;

  IState *node = this;
  while (node != nullptr)
  {
    branch.push_back(dynamic_cast<IState *>(node));
    node = node->m_parent;
  }

  if (!reverse)
    std::reverse(branch.begin(), branch.end());

  return branch;
}

/**
 * @brief Sets the activation of this state and its ancestors.
 * @param active If this state is active or not
 * @param terminateAt Ancestor at which to stop modifying activation
 * @param delta The change of state (last state if active=true, next state if
 *              active=false)
 */
void IState::setActivation(bool active, IState *terminateAt, IState *delta)
{
  if (terminateAt == this)
    return;

  if (!active)
    onExit(delta);

  if (m_parent)
  {
    m_parent->m_active = active ? this : nullptr;
    m_parent->setActivation(active, terminateAt);
  }

  if (active)
    onEntry(delta);
}

/**
 * @brief Test for transfer conditions from this state to another.
 * @return The IState to transfer to, nullptr if no transfer conditions are met
 */
IState *IState::testTransferFrom() const
{
  IState *retVal = nullptr;

  for (auto it = m_transferFromTests.begin(); it != m_transferFromTests.end(); ++it)
  {
    retVal = it->operator()();

    if (retVal != nullptr)
      break;
  }

  return retVal;
}

/**
 * @brief Test for transfer conditions from a sibling state to this state.
 * @return True if the transfer conditions are met.
 */
bool IState::testTransferTo() const
{
  for (auto it = m_transferToTests.begin(); it != m_transferToTests.end(); ++it)
  {
    if (it->operator()())
      return true;
  }

  return false;
}

/**
 * @brief Performs actions required when entering this state.
 * @param last Last state to be active
 */
void IState::onEntry(IState *last)
{
  for (auto it = m_onEntryBehaviours.begin(); it != m_onEntryBehaviours.end(); ++it)
    it->operator()(last);
}

/**
 * @brief Performs actions required when leaving this state.
 * @param next State that is going to be entered next
 */
void IState::onExit(IState *next)
{
  for (auto it = m_onExitBehaviours.begin(); it != m_onExitBehaviours.end(); ++it)
    it->operator()(next);
}

/**
 * @brief Perform the operations that define the behaviour of this state.
 */
void IState::onOperate()
{
  for (auto it = m_onOperateBehaviours.begin(); it != m_onOperateBehaviours.end(); ++it)
    it->operator()();
}
