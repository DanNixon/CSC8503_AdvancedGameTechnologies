#include "NetSyncStateMachine.h"

/**
 * @brief Creates a new synchronised state machine.
 * @param broker Broker to connect to
 * @param rxTopic Topic to listen for state changes on
 * @param txTopic Topic to broadcast state changes to
 */
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

/**
 * @copydoc StateMachine:;Transfer
 */
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
    const char *msg = branchStr.c_str();
    m_broker.BroadcastMessage(this, m_txTopic, msg, (uint16_t)strlen(msg));
  }

  m_txBlanking = false;

  return didTransfer;
}

/**
 * @copydoc IPubSubClient::HandleSubscription
 */
bool NetSyncStateMachine::HandleSubscription(const std::string &topic, const char *msg, uint16_t len)
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
