#include "NetSyncStateMachine.h"

NetSyncStateMachine::NetSyncStateMachine()
{
}

NetSyncStateMachine::~NetSyncStateMachine()
{
}

bool NetSyncStateMachine::NeedsNetworkSync() const
{
  return ActiveStateBranch().back() == m_lastKnownState;
}

void NetSyncStateMachine::BuildNetworkSyncPacket(StateMachineUpdatePacket &pkt) const
{
  // TODO
}

bool NetSyncStateMachine::ProcessNetworkSyncPacket(const StateMachineUpdatePacket &pkt)
{
  // TODO
  return false;
}
