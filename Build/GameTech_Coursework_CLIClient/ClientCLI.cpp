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
      if (m_broker != nullptr)
      {
        out << "Already connected to a server.\n";
        return 1;
      }

      {
        // Create and init broker
        m_broker = new PubSubBrokerNetNode();
        if (!m_broker->Initialize(std::stoi(argv[1]), 16))
        {
          out << "Broker node init failed.\n";
          delete m_broker;
          return 1;
        }

        // Connect to server
        uint8_t ip[4];
        Utility::ParseIPAddress(ip, argv[2]);
        m_broker->ConnectToBroker(ip, std::stoi(argv[3]));
        out << "Connected to server.\n";

        // Create local pub/sub client
        this->m_pubSubClients["basic"] = new FunctionalPubSubClient(m_broker);
        this->m_pubSubClients["basic"]->SetSubscriptionHandler([](const std::string &topic, const char *msg, uint16_t) {
          printf("MSG: %s = %s\n", topic.c_str(), msg);
          return true;
        });

        // Start network update thread
        m_networkUpdateThread = std::thread(&ClientCLI::NetworkUpdateLoop, this);
      }

      return COMMAND_EXIT_CLEAN;
    };

    registerCommand(
        std::make_shared<Command>("connect", func, 4, "Connects to a server instance. [my port] [server IP] [server port]"));
  }

  // Disconnect command
  {
    auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
      if (m_broker == nullptr)
      {
        out << "Not connected to a server.\n";
        return 1;
      }

      PubSubBrokerNetNode *broker = m_broker;
      {
        std::lock_guard<std::mutex> lock(m_broker->Mutex());

        // Remove all clients
        for (auto it = m_pubSubClients.begin(); it != m_pubSubClients.end(); ++it)
        {
          m_broker->UnSubscribeFromAll(it->second);
          delete it->second;
        }
        m_pubSubClients.clear();

        // Close broker network node
        m_broker->Release();
        m_broker = nullptr;
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
      if (m_broker == nullptr)
      {
        out << "Not connected to a server.\n";
        return 1;
      }

      // Generate message
      std::vector<std::string> messageParts(argv.begin() + 1, argv.end());
      std::string msg = Utility::Join(messageParts, ' ');

      // Broadcast message
      {
        std::lock_guard<std::mutex> lock(m_broker->Mutex());
        m_broker->BroadcastMessage(nullptr, argv[1], msg.c_str(), (uint16_t)msg.size());
      }

      out << "Published to topic \"" << argv[1] << "\": " << msg << "\n";
      return COMMAND_EXIT_CLEAN;
    };

    registerCommand(std::make_shared<Command>("publish", func, 3, "Publishes a string message to a topic."));
  }

  // Subscribe command
  {
    auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
      if (m_broker == nullptr)
      {
        out << "Not connected to a server.\n";
        return 1;
      }

      // Subscribe local pub/sub client to a topic
      {
        std::lock_guard<std::mutex> lock(m_broker->Mutex());
        m_broker->Subscribe(m_pubSubClients["basic"], argv[1]);
      }

      out << "Subscribed to topic \"" << argv[1] << "\"\n";
      return COMMAND_EXIT_CLEAN;
    };

    registerCommand(std::make_shared<Command>("subscribe", func, 2, "Subscribes to a topic."));
  }

  // Physics specific commands
  {
    SubCommand_ptr physicsCommands = std::make_shared<SubCommand>("physics", "Physics specific commands.");
    registerCommand(physicsCommands);

    // Toggle pause
    {
      auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
        if (m_broker == nullptr)
        {
          out << "Not connected to a server.\n";
          return 1;
        }

        // Generate message
        char msg = Utility::StringToBool(argv[1], true) ? 'R' : 'P';

        // Broadcast message
        {
          std::lock_guard<std::mutex> lock(m_broker->Mutex());
          m_broker->BroadcastMessage(m_pubSubClients["basic"], "physics/pause", &msg, 1);
        }

        return COMMAND_EXIT_CLEAN;
      };

      physicsCommands->registerCommand(std::make_shared<Command>("pause", func, 2, "Toggles pausing of physics engine."));
    }

    // TODO
    // TEMPLATE
    {
      auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
        if (m_broker == nullptr)
        {
          out << "Not connected to a server.\n";
          return 1;
        }

        // TODO

        // Broadcast message
        {
          std::lock_guard<std::mutex> lock(m_broker->Mutex());
          //m_broker->BroadcastMessage(m_pubSubClients["basic"], "", , );
        }

        return COMMAND_EXIT_CLEAN;
      };

      physicsCommands->registerCommand(std::make_shared<Command>("template", func, 1, ""));
    }
  }

  // Game specific commands
  {
    SubCommand_ptr gameCommands = std::make_shared<SubCommand>("game", "Game specific commands.");
    registerCommand(gameCommands);

    // Announce message
    {
      auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
        if (m_broker == nullptr)
        {
          out << "Not connected to a server.\n";
          return 1;
        }

        // Generate message
        std::vector<std::string> messageParts(argv.begin() + 1, argv.end());
        std::string msg = Utility::Join(messageParts, ' ');

        // Broadcast message
        {
          std::lock_guard<std::mutex> lock(m_broker->Mutex());
          m_broker->BroadcastMessage(m_pubSubClients["basic"], "announce", msg.c_str(), (uint16_t)msg.size());
        }

        return COMMAND_EXIT_CLEAN;
      };

      gameCommands->registerCommand(std::make_shared<Command>("announce", func, 2, "Announces a message to all clients."));
    }

    // List highscore command
    {
      auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
        if (m_broker == nullptr)
        {
          out << "Not connected to a server.\n";
          return 1;
        }

        // Create the client if it does not exist
        if (this->m_pubSubClients["highscores"] == nullptr)
        {
          m_pubSubClients["highscores"] = new FunctionalPubSubClient(m_broker);
          m_broker->Subscribe(m_pubSubClients["highscores"], "highscore/list");
          m_pubSubClients["highscores"]->SetSubscriptionHandler([&out](const std::string &topic, const char *msg, uint16_t) {
            std::vector<std::string> tokens = Utility::Split(std::string(msg), ',');

            if (tokens.empty())
            {
              out << "No scores!\n";
            }
            else
            {
              out << "Top scores:\n";
              for (size_t i = 0; i < tokens.size(); i += 2)
              {
                std::string name = tokens[i];
                if (name.empty())
                  name = "[no name]";

                out << "  " << (i / 2) + 1 << ") " << name << " - " << tokens[i + 1] << '\n';
              }
            }

            return true;
          });
        }

        // Broadcast message
        {
          std::lock_guard<std::mutex> lock(m_broker->Mutex());
          m_broker->BroadcastMessage(m_pubSubClients["highscores"], "highscore/list", argv[1].c_str(), (uint16_t)argv[1].size());
        }

        return COMMAND_EXIT_CLEAN;
      };

      gameCommands->registerCommand(std::make_shared<Command>("highscores", func, 2, "Lists top scores from server."));
    }

    // Save highscores command
    {
      auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
        if (m_broker == nullptr)
        {
          out << "Not connected to a server.\n";
          return 1;
        }

        // Create the client if it does not exist
        if (this->m_pubSubClients["savescores"] == nullptr)
        {
          m_pubSubClients["savescores"] = new FunctionalPubSubClient(m_broker);
          m_broker->Subscribe(m_pubSubClients["savescores"], "highscore/save");
          m_pubSubClients["savescores"]->SetSubscriptionHandler([&out](const std::string &topic, const char *msg, uint16_t) {
            if (msg[0] == '1')
              out << "Saved server scoreboard.\n";
            else
              out << "Failed to save server scoreboard!\n";

            return true;
          });
        }

        // Broadcast message
        {
          std::lock_guard<std::mutex> lock(m_broker->Mutex());
          m_broker->BroadcastMessage(m_pubSubClients["savescores"], "highscore/save", "S", 1);
        }

        return COMMAND_EXIT_CLEAN;
      };

      gameCommands->registerCommand(std::make_shared<Command>("savescores", func, 1, "Saves highscores on server from file."));
    }

    // Load highscores command
    {
      auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
        if (m_broker == nullptr)
        {
          out << "Not connected to a server.\n";
          return 1;
        }

        // Create the client if it does not exist
        if (this->m_pubSubClients["loadscores"] == nullptr)
        {
          m_pubSubClients["loadscores"] = new FunctionalPubSubClient(m_broker);
          m_broker->Subscribe(m_pubSubClients["loadscores"], "highscore/load");
          m_pubSubClients["loadscores"]->SetSubscriptionHandler([&out](const std::string &topic, const char *msg, uint16_t) {
            if (msg[0] == '1')
              out << "Loaded server scoreboard.\n";
            else
              out << "Failed to load server scoreboard!\n";

            return true;
          });
        }

        // Broadcast message
        {
          std::lock_guard<std::mutex> lock(m_broker->Mutex());
          m_broker->BroadcastMessage(m_pubSubClients["loadscores"], "highscore/load", "L", 1);
        }

        return COMMAND_EXIT_CLEAN;
      };

      gameCommands->registerCommand(std::make_shared<Command>("loadscores", func, 1, "Loads highscores on server from file."));
    }

    // "Give player ammo" command
    {
      auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
        if (m_broker == nullptr)
        {
          out << "Not connected to a server.\n";
          return 1;
        }

        // Broadcast message
        {
          std::lock_guard<std::mutex> lock(m_broker->Mutex());
          m_broker->BroadcastMessage(m_pubSubClients["basic"], "player/ammo_delta", (void*)argv[1].c_str(), (uint16_t) argv[1].size());
        }

        return COMMAND_EXIT_CLEAN;
      };

      gameCommands->registerCommand(std::make_shared<Command>("ammogive", func, 2, "Gives (or takes away) ammo from the player."));
    }

    // Set shootable lifetime command
    {
      auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
        if (m_broker == nullptr)
        {
          out << "Not connected to a server.\n";
          return 1;
        }

        // Generate message
        float msg = std::stof(argv[1]);

        // Broadcast message
        {
          std::lock_guard<std::mutex> lock(m_broker->Mutex());
          m_broker->BroadcastMessage(m_pubSubClients["basic"], "player/shootable_lifetime", (void*)&msg, (uint16_t)sizeof(float));
        }

        return COMMAND_EXIT_CLEAN;
      };

      gameCommands->registerCommand(std::make_shared<Command>("shootablelife", func, 2, "Sets the lifetime of shootable objects (in seconds)."));
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
