#pragma once

#include <string>
#include <vector>

class IPubSubClient;

class PubSubBroker
{
public:
  struct Subscription
  {
    IPubSubClient *client;
    std::string topic;

    bool operator==(const Subscription &other)
    {
      return (client == other.client) && (topic == other.topic);
    }
  };

public:
  PubSubBroker();
  virtual ~PubSubBroker();

  void Subscribe(IPubSubClient *client, const std::string &topic);
  bool UnSubscribe(IPubSubClient *client, const std::string &topic);

  void BroadcastMessage(const std::string &topic, const char *msg);

protected:
  std::vector<Subscription> m_subscriptions;
};