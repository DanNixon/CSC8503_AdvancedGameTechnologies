#include "PubSubBroker.h"

#include "IPubSubClient.h"

/**
 * @brief Creates a new broker.
 */
PubSubBroker::PubSubBroker()
{
}

PubSubBroker::~PubSubBroker()
{
}

/**
 * @brief Subscribes a client to a topic on this broker.
 * @param client Pointer to client
 * @param topic Topic to subscribe to
 */
void PubSubBroker::Subscribe(IPubSubClient *client, const std::string &topic)
{
  m_subscriptions.push_back({client, topic});
}

/**
 * @brief Unsubscribes a client form a given topic on this broker.
 * @param client Pointer to client
 * @param topic Topic to unsubscribe from
 * @return True if unsubscribe was successful
 */
bool PubSubBroker::UnSubscribe(IPubSubClient *client, const std::string &topic)
{
  auto it = std::find(m_subscriptions.begin(), m_subscriptions.end(), Subscription({client, topic}));
  bool found = (it != m_subscriptions.end());
  if (found)
    m_subscriptions.erase(it);
  return found;
}

/**
 * @brief Broadcasts a message to all clients subscribed to a topic.
 * @param topic Topic to publish on
 * @param msg Message payload
 */
void PubSubBroker::BroadcastMessage(const std::string &topic, const char *msg)
{
  for (auto it = m_subscriptions.begin(); it != m_subscriptions.end(); ++it)
  {
    if (it->topic == topic)
      it->client->HandleSubscription(topic, msg);
  }
}
