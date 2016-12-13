#pragma once

#include "NetworkBase.h"
#include "PubSubBroker.h"

class PubSubBrokerNetNode : public PubSubBroker, public NetworkBase
{
public:
  PubSubBrokerNetNode();
  virtual ~PubSubBrokerNetNode();

  bool ConnectToBroker(const uint8_t ip[4], uint16_t port);

  void PumpNetwork(float dt);

  virtual void BroadcastMessage(IPubSubClient *source, const std::string &topic, const char *msg, uint16_t len);

protected:
  void HandleRxEvent(const ENetEvent &rxEvent);

protected:
  std::vector<ENetPeer *> m_connections;
};