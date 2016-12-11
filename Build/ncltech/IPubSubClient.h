#pragma once

#include "PubSubBroker.h"

#include <string>

class IPubSubClient
{
public:
  IPubSubClient(PubSubBroker &broker)
      : m_broker(broker)
  {
  }

  virtual ~IPubSubClient()
  {
  }

  virtual bool HandleSubscription(const std::string &topic, const char *msg) = 0;

protected:
  PubSubBroker &m_broker; //!< Broker this client is attached to
};