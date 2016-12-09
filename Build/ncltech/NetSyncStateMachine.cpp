#include "NetSyncStateMachine.h"

NetSyncStateMachine::NetSyncStateMachine()
    : m_lastSyncState(nullptr)
{
}

NetSyncStateMachine::~NetSyncStateMachine()
{
}

bool NetSyncStateMachine::NeedsNetworkSync() const
{
  StatePtrList branch = ActiveStateBranch();
  return branch.empty() || branch.back() != m_lastSyncState;
}

void NetSyncStateMachine::BuildNetworkSyncPacket(StateMachineUpdatePacket &pkt)
{
  // Get target state string
  StatePtrList branch = ActiveStateBranch();
  std::string branchStr = BranchToString(branch, '/');

  // Build packet
  pkt.type = PACKET_TYPE_FSM;
  strncpy_s(pkt.nextStateName, branchStr.c_str(), 1204);

  // Mark last known synced state
  m_lastSyncState = branch.back();
}

bool NetSyncStateMachine::ProcessNetworkSyncPacket(const StateMachineUpdatePacket &pkt)
{
  // Check message type
  if (pkt.type != PACKET_TYPE_FSM)
    return false;

  // Find target state
  std::string targetStateName(pkt.nextStateName);
  State *targetState = m_root.FindState(targetStateName, '/').back();
  if (targetState == nullptr)
    return false;

  // Activate state
  targetState->SetActivation(true);

  // Mark last known synced state
  m_lastSyncState = targetState;

  return true;
}
