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
 * @brief Removes all subscriptions a client has.
 * @param client Client who's subscriptions are to be removed
 */
 void PubSubBroker::UnSubscribeFromAll(IPubSubClient * client)
 {
   for (auto it = m_subscriptions.begin(); it != m_subscriptions.end();)
   {
     if (it->client == client)
       it = m_subscriptions.erase(it);
     else
       ++it;
   }
 }

/**
 * @brief Broadcasts a message to all clients subscribed to a topic.
 * @param source Source client
 * @param topic Topic to publish on
 * @param msg Message payload
 * @param len Length of message payload
 */
void PubSubBroker::BroadcastMessage(IPubSubClient *source, const std::string &topic, const char *msg, uint16_t len)
{
  // Drop empty messages
  if (len == 0)
    return;

  // Broadcast messages
  for (auto it = m_subscriptions.begin(); it != m_subscriptions.end(); ++it)
  {
    if (it->topic == topic && it->client != source)
      it->client->HandleSubscription(topic, msg, len);
  }
}
