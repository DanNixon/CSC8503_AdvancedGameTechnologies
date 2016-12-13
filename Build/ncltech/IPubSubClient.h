#pragma once

#include "PubSubBroker.h"

#include <string>

/**
 * @class IPubSubClient
 * @author Dan Nixon
 * @brief Interface for clients to a publisher/subscriber broker.
 */
class IPubSubClient
{
public:
  /**
   * @brief Creates a new client.
   * @param broker Broker to connect to
   */
  IPubSubClient(PubSubBroker &broker)
      : m_broker(broker)
  {
  }

  virtual ~IPubSubClient()
  {
    m_broker.UnSubscribeFromAll(this);
  }

  /**
   * @brief Handles delivery of a new message from a subscribed topic.
   * @param topic Topic
   * @param msg Message payload
   * @param len Length of message payload
   * @return Flag indicating successful handling of the message
   */
  virtual bool HandleSubscription(const std::string &topic, const char *msg, uint16_t len) = 0;

protected:
  PubSubBroker &m_broker; //!< Broker this client is attached to
};
