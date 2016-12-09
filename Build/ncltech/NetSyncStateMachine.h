#pragma once

#include "StateMachine.h"

class NetSyncStateMachine : public StateMachine
{
public:
  struct StateMachineUpdatePacket
  {
    char nextStateName[1024];
  };

public:
  NetSyncStateMachine();
  virtual ~NetSyncStateMachine();

  bool NeedsNetworkSync() const;
  void BuildNetworkSyncPacket(StateMachineUpdatePacket &pkt) const;
  bool ProcessNetworkSyncPacket(const StateMachineUpdatePacket &pkt);

private:
  State *m_lastKnownState; //!< Pointer to the last known active state
};
