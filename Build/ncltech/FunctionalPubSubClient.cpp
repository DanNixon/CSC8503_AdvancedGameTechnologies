#include "FunctionalPubSubClient.h"

FunctionalPubSubClient::FunctionalPubSubClient(PubSubBroker &broker)
    : IPubSubClient(broker)
{
}

FunctionalPubSubClient::~FunctionalPubSubClient()
{
}

void FunctionalPubSubClient::SetSubscriptionHandler(HandleSubscruptionFunction handler)
{
  m_handler = handler;
}

bool FunctionalPubSubClient::HandleSubscription(const std::string &topic, const char *msg, uint16_t len)
{
  
}