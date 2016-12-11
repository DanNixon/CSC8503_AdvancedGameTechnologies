#include "PubSubBroker.h"

#include "IPubSubClient.h"

PubSubBroker::PubSubBroker()
{
}

PubSubBroker::~PubSubBroker()
{
}

void PubSubBroker::Subscribe(IPubSubClient *client, const std::string &topic)
{
  m_subscriptions.push_back({client, topic});
}

bool PubSubBroker::UnSubscribe(IPubSubClient *client, const std::string &topic)
{
  auto it = std::find(m_subscriptions.begin(), m_subscriptions.end(), Subscription({client, topic}));
  bool found = (it != m_subscriptions.end());
  if (found)
    m_subscriptions.erase(it);
  return found;
}

void PubSubBroker::BroadcastMessage(const std::string &topic, const char *msg)
{
  for (auto it = m_subscriptions.begin(); it != m_subscriptions.end(); ++it)
  {
    if (it->topic == topic)
      it->client->HandleSubscription(topic, msg);
  }
}
