#pragma once

#include "NetworkBase.h"
#include "PubSubBroker.h"

#include <mutex>

/**
 * @class PubSubBrokerNetNode
 * @author Dan Nixon
 * @brief Publisher/subscriber broker that synchronises with other brokers over the network.
 */
class PubSubBrokerNetNode : public PubSubBroker, public NetworkBase
{
public:
  PubSubBrokerNetNode();
  virtual ~PubSubBrokerNetNode();

  /**
   * @brief Gets a mutex that may be used to make calls to this broker thread safe.
   * @return Reference to mutex
   *
   * This is done manually rather than mutex locing each call as it may be desirable to hold the lock for the duration of several
   * operations (for instance when closing the network).
   */
  inline std::mutex &Mutex()
  {
    return m_netMutex;
  }

  void ConnectToBroker(const uint8_t ip[4], uint16_t port);
  bool IsConnectedToServer() const;

  void PumpNetwork(float dt);

  virtual void BroadcastMessage(IPubSubClient *source, const std::string &topic, const char *msg, uint16_t len) override;

protected:
  void HandleRxEvent(const ENetEvent &rxEvent);

  void BroadcastMessageIgnorePeer(IPubSubClient *source, const std::string &topic, const char *msg, uint16_t len,
                                  int32_t ignorePeer);

protected:
  std::mutex m_netMutex;                 //!< Mutex controling access
  std::vector<ENetPeer *> m_connections; //!< Active connection
};
