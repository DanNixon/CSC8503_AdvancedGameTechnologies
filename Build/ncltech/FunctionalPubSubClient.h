#pragma once

#include "IPubSubClient.h"

#include <functional>

/**
 * @class FunctionalPubSubClient
 * @author Dan Nixon
 * @brief Publisher/subscriber client that uses a functional callback for subscription handling.
 */
class FunctionalPubSubClient : public IPubSubClient
{
public:
  typedef std::function<bool(const std::string &, const char *, uint16_t)> HandleSubscruptionFunction;

public:
  /**
   * @brief Creates a new functional pub/sub client.
   * @param broker Pointer to broker
   */
  FunctionalPubSubClient(PubSubBroker *broker)
      : IPubSubClient(broker)
  {
  }

  virtual ~FunctionalPubSubClient()
  {
  }

  /**
   * @brief Sets the subscription handler function.
   * @param handler Handler function
   */
  inline void SetSubscriptionHandler(HandleSubscruptionFunction handler)
  {
    m_handler = handler;
  }

  /**
   * @copydoc IPubSubClient::HandleSubscription
   */
  virtual bool HandleSubscription(const std::string &topic, const char *msg, uint16_t len)
  {
    return m_handler ? m_handler(topic, msg, len) : false;
  }

protected:
  HandleSubscruptionFunction m_handler; //!< Subscription handler
};
