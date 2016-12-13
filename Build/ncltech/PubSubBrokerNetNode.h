#pragma once

#include "NetworkBase.h"
#include "PubSubBroker.h"

#include <mutex>

class PubSubBrokerNetNode : public PubSubBroker, public NetworkBase
{
public:
  PubSubBrokerNetNode();
  virtual ~PubSubBrokerNetNode();

  inline std::mutex &Mutex()
  {
    return m_netMutex;
  }

  void ConnectToBroker(const uint8_t ip[4], uint16_t port);
  bool IsConnectedToServer() const;

  void PumpNetwork(float dt);

  virtual void BroadcastMessage(IPubSubClient *source, const std::string &topic, const char *msg, uint16_t len,
                                int32_t ignorePeer = -1);

protected:
  void HandleRxEvent(const ENetEvent &rxEvent);

protected:
  std::mutex m_netMutex;                 //!< Mutex controling access
  std::vector<ENetPeer *> m_connections; //!< Active connection
};