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

  virtual void Subscribe(IPubSubClient *client, const std::string &topic);
  virtual bool UnSubscribe(IPubSubClient *client, const std::string &topic);

  virtual void BroadcastMessage(const std::string &topic, const char *msg);

protected:
  std::vector<Subscription> m_subscriptions;
};