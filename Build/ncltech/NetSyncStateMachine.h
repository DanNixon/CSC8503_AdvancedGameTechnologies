#pragma once

#include "StateMachine.h"

#include "NetworkBase.h"

class NetSyncStateMachine : public StateMachine
{
public:
  struct StateMachineUpdatePacket : public NetPacket
  {
    char nextStateName[1024];
  };

public:
  NetSyncStateMachine();
  virtual ~NetSyncStateMachine();

  bool NeedsNetworkSync() const;
  void BuildNetworkSyncPacket(StateMachineUpdatePacket &pkt);
  bool ProcessNetworkSyncPacket(const StateMachineUpdatePacket &pkt);

private:
  State *m_lastSyncState;
};
