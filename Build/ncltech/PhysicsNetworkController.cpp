#include "PhysicsNetworkController.h"

#include "PhysicsEngine.h"

PhysicsNetworkController::PhysicsNetworkController(PubSubBroker * broker)
  : IPubSubClient(broker)
{
  // Subscribe to topics
  if (broker != nullptr)
  {
    broker->Subscribe(this, "physics/pause");
    // TODO
  }
}

PhysicsNetworkController::~PhysicsNetworkController()
{
}

/**
 * @copydoc IPubSubClient::HandleSubscription
 */
bool PhysicsNetworkController::HandleSubscription(const std::string & topic, const char * msg, uint16_t len)
{
  if (topic == "physics/pause")
  {
    // Handle pause/resume
    PhysicsEngine::Instance()->SetPaused(((char *)msg)[0] == 'P');
  }
  // TODO
  else
  {
    return false;
  }

  return true;
}
