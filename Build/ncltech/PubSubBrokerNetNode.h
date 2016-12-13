#pragma once

#include "PubSubBroker.h"
#include "NetworkBase.h"

class PubSubBrokerNetNode : public PubSubBroker, public NetworkBase
{
public:
  PubSubBrokerNetNode();
  virtual ~PubSubBrokerNetNode();

  bool ConnectToBroker(const uint8_t ip[4], uint16_t port);

  void PumpNetwork(float dt);

//protected:
  void HandleRxEvent(const ENetEvent &rxEvent);
  void NetworkTxPayload(const std::string &topic, const char * msg, uint16_t len);

protected:
  std::vector<ENetPeer *> m_connections;
};