#include "ClientCLI.h"

#include <CommandLineInterfaceLib\SubCommand.h>
#include <ncltech\Utility.h>

/**
 * @brief Creates a new client CLI.
 * @param in Reference to input stream
 * @param out Reference to output stream
 */
ClientCLI::ClientCLI(std::istream &in, std::ostream &out)
    : CLI(in, out)
{
}

ClientCLI::~ClientCLI()
{
  // Remove all clients
  for (auto it = m_pubSubClients.begin(); it != m_pubSubClients.end(); ++it)
    delete it->second;
  m_pubSubClients.clear();
}

/**
 * @brief Creates commands for the CLI.
 */
void ClientCLI::InitCLI()
{
  // Connect command
  {
    auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
      if (this->m_broker != nullptr)
      {
        out << "Already connected to a server.\n";
        return 1;
      }

      {
        // Create and init broker
        this->m_broker = new PubSubBrokerNetNode();
        if (!this->m_broker->Initialize(std::stoi(argv[1]), 16))
        {
          out << "Broker node init failed.\n";
          delete this->m_broker;
          return 1;
        }

        // Connect to server
        uint8_t ip[4];
        Utility::ParseIPAddress(ip, argv[2]);
        this->m_broker->ConnectToBroker(ip, std::stoi(argv[3]));
        out << "Connected to server.\n";

        // Create local pub/sub client
        this->m_pubSubClients["basic"] = new FunctionalPubSubClient(this->m_broker);
        this->m_pubSubClients["basic"]->SetSubscriptionHandler([&](const std::string &topic, const char *msg, uint16_t) {
          printf("MSG: %s = %s\n", topic.c_str(), msg);
          return true;
        });

        // Start network update thread
        this->m_networkUpdateThread = std::thread(&ClientCLI::NetworkUpdateLoop, this);
      }

      return COMMAND_EXIT_CLEAN;
    };

    registerCommand(
        std::make_shared<Command>("connect", func, 4, "Connects to a server instance. [my port] [server IP] [server port]"));
  }

  // Disconnect command
  {
    auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
      if (this->m_broker == nullptr)
      {
        out << "Not connected to a server.\n";
        return 1;
      }

      PubSubBrokerNetNode *broker = this->m_broker;
      {
        std::lock_guard<std::mutex> lock(this->m_broker->Mutex());

        // Remove all clients
        for (auto it = m_pubSubClients.begin(); it != m_pubSubClients.end(); ++it)
        {
          this->m_broker->UnSubscribeFromAll(it->second);
          delete it->second;
        }
        m_pubSubClients.clear();

        // Close broker network node
        this->m_broker->Release();
        this->m_broker = nullptr;
      }

      // Stop network update thread
      if (this->m_networkUpdateThread.joinable())
        this->m_networkUpdateThread.join();

      // Free broker
      delete broker;

      out << "Disconnected.\n";
      return COMMAND_EXIT_CLEAN;
    };

    registerCommand(std::make_shared<Command>("disconnect", func, 1, "Disconnects the active server connection."));
  }

  // Publish command
  {
    auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
      if (this->m_broker == nullptr)
      {
        out << "Not connected to a server.\n";
        return 1;
      }

      // Generate message
      std::vector<std::string> messageParts(argv.begin() + 1, argv.end());
      std::string msg = Utility::Join(messageParts, ' ');

      // Broadcast message
      {
        std::lock_guard<std::mutex> lock(this->m_broker->Mutex());
        this->m_broker->BroadcastMessage(nullptr, argv[1], msg.c_str(), (uint16_t)msg.size());
      }

      out << "Published to topic \"" << argv[1] << "\": " << msg << "\n";
      return COMMAND_EXIT_CLEAN;
    };

    registerCommand(std::make_shared<Command>("publish", func, 3, "Publishes a string message to a topic."));
  }

  // Subscribe command
  {
    auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
      if (this->m_broker == nullptr)
      {
        out << "Not connected to a server.\n";
        return 1;
      }

      // Subscribe local pub/sub client to a topic
      {
        std::lock_guard<std::mutex> lock(this->m_broker->Mutex());
        this->m_broker->Subscribe(this->m_pubSubClients["basic"], argv[1]);
      }

      out << "Subscribed to topic \"" << argv[1] << "\"\n";
      return COMMAND_EXIT_CLEAN;
    };

    registerCommand(std::make_shared<Command>("subscribe", func, 2, "Subscribes to a topic."));
  }

  // Game specific commands
  {
    SubCommand_ptr gameCommands = std::make_shared<SubCommand>("game", "Game specific commands.");
    registerCommand(gameCommands);

    // Announce message
    {
      auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
        if (this->m_broker == nullptr)
        {
          out << "Not connected to a server.\n";
          return 1;
        }

        // Generate message
        std::vector<std::string> messageParts(argv.begin() + 1, argv.end());
        std::string msg = Utility::Join(messageParts, ' ');

        // Broadcast announce message
        {
          std::lock_guard<std::mutex> lock(this->m_broker->Mutex());
          this->m_broker->BroadcastMessage(this->m_pubSubClients["basic"], "announce", msg.c_str(), (uint16_t)msg.size());
        }

        return COMMAND_EXIT_CLEAN;
      };

      gameCommands->registerCommand(std::make_shared<Command>("announce", func, 2, "Announces a message to all clients."));
    }

    // List highscore command
    {
      auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
        // TODO
        return COMMAND_EXIT_CLEAN;
      };

      gameCommands->registerCommand(std::make_shared<Command>("highscores", func, 2, "Lists top scores from server."));
    }

    // Save highscores command
    {
      auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
        // TODO
        return COMMAND_EXIT_CLEAN;
      };

      gameCommands->registerCommand(std::make_shared<Command>("savescores", func, 1, "Saves highscores on server from file."));
    }

    // Load highscores command
    {
      auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
        // TODO
        return COMMAND_EXIT_CLEAN;
      };

      gameCommands->registerCommand(std::make_shared<Command>("loadscores", func, 1, "Loads highscores on server from file."));
    }
  }
}

/**
 * @brief Runs the update loop required to operate the networking componenet of the client.
 */
void ClientCLI::NetworkUpdateLoop()
{
  while (m_broker != nullptr)
  {
    {
      std::lock_guard<std::mutex> lock(m_broker->Mutex());

      // Do another text just in case broker was deleted while waiting for mutex lock
      if (m_broker != nullptr)
      {
        float dt = m_networkUpdateTimer.GetTimedMS() * 0.001f;
        m_broker->PumpNetwork(dt);
      }
    }

    Sleep(0);
  }
}
