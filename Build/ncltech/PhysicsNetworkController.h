#pragma once

#include "IPubSubClient.h"

class PhysicsNetworkController : public IPubSubClient
{
public:
  static const std::string INV_MASS_NAME;
  static const std::string POSITION_NAME;
  static const std::string LINEAR_VELOCITY_NAME;
  static const std::string FORCE_NAME;
  static const std::string ORIENTATION_NAME;
  static const std::string ANGULAR_VEL_NAME;
  static const std::string TORQUE_NAME;

public:
  PhysicsNetworkController(PubSubBroker *broker);
  virtual ~PhysicsNetworkController();

  virtual bool HandleSubscription(const std::string &topic, const char *msg, uint16_t len) override;
};