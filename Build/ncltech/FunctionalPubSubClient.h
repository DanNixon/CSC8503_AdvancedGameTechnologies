#pragma once

#include "IPubSubClient.h"

#include <functional>

class FunctionalPubSubClient : public IPubSubClient
{
public:
  typedef std::function<bool(const std::string &, const char *, uint16_t)> HandleSubscruptionFunction;

public:
  FunctionalPubSubClient(PubSubBroker &broker)
    : IPubSubClient(broker)
  {
  }

  virtual ~FunctionalPubSubClient()
  {
  }

  inline void SetSubscriptionHandler(HandleSubscruptionFunction handler)
  {
    m_handler = handler;
  }

  virtual bool HandleSubscription(const std::string &topic, const char *msg, uint16_t len)
  {
    return m_handler ? m_handler(topic, msg, len) : false;
  }

protected:
  HandleSubscruptionFunction m_handler;
};

