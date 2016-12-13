#include "ClientCLI.h"

#include <ncltech\Utility.h>

ClientCLI::ClientCLI(std::istream &in, std::ostream &out)
    : CLI(in, out)
{
}

ClientCLI::~ClientCLI()
{
}

void ClientCLI::InitCLI()
{
  // Connect command
  registerCommand(std::make_shared<Command>("connect",
                                            [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
                                              if (this->m_broker != nullptr)
                                              {
                                                out << "Already connected to a server.\n";
                                                return 1;
                                              }

                                              {
                                                std::lock_guard<std::mutex> lock(m_networkMutex);

                                                // Create and init broker
                                                this->m_broker = new PubSubBrokerNetNode();
                                                if (!this->m_broker->Initialize(std::stoi(argv[1]), 16))
                                                {
                                                  out << "An error occurred while trying to create an ENet server host.\n";
                                                  delete this->m_broker;
                                                  return 1;
                                                }

                                                // Connect to server
                                                uint8_t ip[4];
                                                Utility::ParseIPAddress(ip, argv[2]);
                                                this->m_broker->ConnectToBroker(ip, std::stoi(argv[3]));
                                                out << "Connected to server.\n";

                                                // Create local pub/sub client
                                                this->m_cliPubSubClient = new FunctionalPubSubClient(*(this->m_broker));
                                                m_cliPubSubClient->SetSubscriptionHandler(
                                                    [&](const std::string &topic, const char *msg, uint16_t) {
                                                      printf("MSG: %s = %s\n", topic.c_str(), msg);
                                                      return true;
                                                    });

                                                // Start network update thread
                                                this->m_networkUpdateThread = std::thread(&ClientCLI::NetworkUpdateLoop, this);
                                              }

                                              return COMMAND_EXIT_CLEAN;
                                            },
                                            4, "Connects to a server instance. [my port] [server IP] [server port]"));

  // Disconnect command
  registerCommand(std::make_shared<Command>("disconnect",
                                            [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
                                              if (this->m_broker == nullptr)
                                              {
                                                out << "Not connected to a server.\n";
                                                return 1;
                                              }

                                              {
                                                std::lock_guard<std::mutex> lock(this->m_networkMutex);

                                                // Remove CLI pub/sub client
                                                this->m_broker->UnSubscribeFromAll(this->m_cliPubSubClient);
                                                delete this->m_cliPubSubClient;

                                                // Close broker network node
                                                this->m_broker->Release();
                                                delete this->m_broker;
                                              }
                                              out << "Disconnected.\n";

                                              // Stop network update thread
                                              if (this->m_networkUpdateThread.joinable())
                                                this->m_networkUpdateThread.join();

                                              out << "Network thread stopped.\n";
                                              return COMMAND_EXIT_CLEAN;
                                            },
                                            1, "Disconnects the active server connection."));

  // Publish command
  registerCommand(std::make_shared<Command>("publish",
                                            [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
                                              if (this->m_broker == nullptr)
                                              {
                                                out << "Not connected to a server.\n";
                                                return 1;
                                              }

                                              // Broadcast a message
                                              const char *msg = argv[2].c_str();

                                              {
                                                std::lock_guard<std::mutex> lock(this->m_networkMutex);
                                                this->m_broker->BroadcastMessage(nullptr, argv[1], msg, (uint16_t)strlen(msg));
                                              }

                                              out << "Published to topic \"" << argv[1] << "\": " << argv[2] << "\n";
                                              return COMMAND_EXIT_CLEAN;
                                            },
                                            3, "Publishes a string message to a topic."));

  // Subscribe command
  registerCommand(std::make_shared<Command>("subscribe",
                                            [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
                                              if (this->m_broker == nullptr)
                                              {
                                                out << "Not connected to a server.\n";
                                                return 1;
                                              }

                                              // Subscribe local pub/sub client to a topic
                                              {
                                                std::lock_guard<std::mutex> lock(this->m_networkMutex);
                                                this->m_broker->Subscribe(m_cliPubSubClient, argv[1]);
                                              }

                                              out << "Subscribed to topic \"" << argv[1] << "\"\n";
                                              return COMMAND_EXIT_CLEAN;
                                            },
                                            2, "Subscribes to a topic."));
}

void ClientCLI::NetworkUpdateLoop()
{
  while(m_broker != nullptr)
  {
    {
      std::lock_guard<std::mutex> lock(m_networkMutex);

      float dt = m_networkUpdateTimer.GetTimedMS() * 0.001f;
      m_broker->PumpNetwork(dt);
    }

    Sleep(0);
  }
}