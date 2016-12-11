#include "NetSyncStateMachine.h"

NetSyncStateMachine::NetSyncStateMachine(PubSubBroker &broker, const std::string &rxTopic, const std::string &txTopic)
    : StateMachine()
    , IPubSubClient(broker)
    , m_txTopic(txTopic)
    , m_txBlanking(false)
{
  m_broker.Subscribe(this, rxTopic);
}

NetSyncStateMachine::~NetSyncStateMachine()
{
}

bool NetSyncStateMachine::Transfer()
{
  bool didTransfer = StateMachine::Transfer();

  // Send message
  if (didTransfer && !m_txBlanking)
  {
    // Get target state string
    StatePtrList branch = ActiveStateBranch();
    std::string branchStr = BranchToString(branch, '/');

    // Send topic update
    m_broker.BroadcastMessage(m_txTopic, branchStr.c_str());
  }

  m_txBlanking = false;

  return didTransfer;
}

bool NetSyncStateMachine::HandleSubscription(const std::string &topic, const char *msg)
{
  // Find target state
  std::string targetStateName(msg);
  State *targetState = m_root.FindState(targetStateName, '/').back();
  if (targetState == nullptr)
    return false;

  // Activate state
  m_txBlanking = true;
  ActivateState(targetState);
  return true;
}