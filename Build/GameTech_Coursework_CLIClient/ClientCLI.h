#pragma once

#include <CommandLineInterfaceLib\CLI.h>

// clang-format off
#include <winsock2.h>
#include <thread>
// clang-format on

#include <nclgl\GameTimer.h>
#include <ncltech\PubSubBrokerNetNode.h>
#include <ncltech\FunctionalPubSubClient.h>
#include <map>

/**
 * @class ClientCLI
 * @author Dan Nixon
 * @brief Provides a command line interface to client functionality over the network.
 */
class ClientCLI : public CLI
{
public:
  ClientCLI(std::istream &in, std::ostream &out);
  virtual ~ClientCLI();

  void InitCLI();

protected:
  void NetworkUpdateLoop();

protected:
  GameTimer m_networkUpdateTimer;    //!< Timer used to time network updates
  PubSubBrokerNetNode *m_broker;     //!< Client broker node
  std::thread m_networkUpdateThread; //!< Thread that runs network update loop

  std::map<std::string, FunctionalPubSubClient *> m_pubSubClients; //!< Pub/sub clients used by the CLI
};