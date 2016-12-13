#pragma once

#include <CommandLineInterfaceLib\CLI.h>

// clang-format off
#include <winsock2.h>
#include <thread>
// clang-format on

#include <nclgl\GameTimer.h>
#include <ncltech\PubSubBrokerNetNode.h>
#include <ncltech\FunctionalPubSubClient.h>

class ClientCLI : public CLI
{
public:
  ClientCLI(std::istream &in, std::ostream &out);
  virtual ~ClientCLI();

  void InitCLI();

protected:
  void NetworkUpdateLoop();

protected:
  GameTimer m_networkUpdateTimer;
  PubSubBrokerNetNode *m_broker;
  FunctionalPubSubClient *m_cliPubSubClient;

  std::thread m_networkUpdateThread;
};