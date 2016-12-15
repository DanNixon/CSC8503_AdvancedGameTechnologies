#include "PhysicsNetworkController.h"

#include "PhysicsEngine.h"

PhysicsNetworkController::PhysicsNetworkController(PubSubBroker * broker)
  : IPubSubClient(broker)
{
  // Subscribe to topics
  if (broker != nullptr)
  {
    broker->Subscribe(this, "physics/pause");
  }
}

PhysicsNetworkController::~PhysicsNetworkController()
{
}

bool PhysicsNetworkController::HandleSubscription(const std::string & topic, void * msg, uint16_t len)
{
  if (topic == "pause")
  {
    PhysicsEngine::Instance()->SetPaused(((char *)msg)[0] == 'P');
  }
  else
  {
    return false;
  }

  return true;
}
