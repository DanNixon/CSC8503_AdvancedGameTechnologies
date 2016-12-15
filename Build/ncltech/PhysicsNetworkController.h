#pragma once

#include "IPubSubClient.h"

class PhysicsNetworkController : public IPubSubClient
{
public:
  PhysicsNetworkController(PubSubBroker *broker);
  virtual ~PhysicsNetworkController();

  virtual bool HandleSubscription(const std::string &topic, const char *msg, uint16_t len) override;
};