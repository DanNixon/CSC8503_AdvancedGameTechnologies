#include "ClientCLI.h"

#include <CommandLineInterfaceLib/SubCommand.h>
#include <nclgl/Quaternion.h>
#include <nclgl/Vector3.h>
#include <ncltech/PhysicsNetworkController.h>
#include <ncltech/Utility.h>
#include <sstream>

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
  static std::map<std::string, char> VALUE_TYPES = {
      {PhysicsNetworkController::INV_MASS_NAME, 'F'},        {PhysicsNetworkController::POSITION_NAME, '3'},
      {PhysicsNetworkController::LINEAR_VELOCITY_NAME, '3'}, {PhysicsNetworkController::FORCE_NAME, '3'},
      {PhysicsNetworkController::ORIENTATION_NAME, 'Q'},     {PhysicsNetworkController::ANGULAR_VEL_NAME, '3'},
      {PhysicsNetworkController::TORQUE_NAME, '3'}};

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
        m_pubSubClients["basic"] = new FunctionalPubSubClient(m_broker);
        m_pubSubClients["basic"]->SetSubscriptionHandler([](const std::string &topic, const char *msg, uint16_t) {
          printf("MSG: %s = %s\n", topic.c_str(), msg);
          return true;
        });

        // Create physics pub/sub client
        m_pubSubClients["physics"] = new FunctionalPubSubClient(m_broker);
        m_pubSubClients["physics"]->SetSubscriptionHandler([](const std::string &topic, const char *msg, uint16_t) {
          std::vector<std::string> tokens = Utility::Split(topic, '/');

          // Ignore non physics messages
          if (tokens[0] != "physics")
            return false;

          // Is an object update message?
          if (tokens[1] == "objects")
          {
            std::stringstream ss;
            ss << tokens[2] << '.' << tokens[3] << " = ";

            switch (VALUE_TYPES[tokens[3]])
            {
            // Value is float
            case 'F':
            {
              float value;
              memcpy(&value, msg, sizeof(float));
              ss << value;
              break;
            }
            // Value is 3 element vector
            case '3':
            {
              Vector3 value;
              memcpy(&value, msg, sizeof(Vector3));
              ss << value;
              break;
            }
            // Value is quaternion
            case 'Q':
            {
              Quaternion value;
              memcpy(&value, msg, sizeof(Quaternion));
              ss << value;
              break;
            }
            default:
              return false;
            }

            ss << '\n';
            printf(ss.str().c_str());
            return true;
          }
          // Is a collision notification message?
          else if (tokens[1] == "collision")
          {
            printf("Object \"%s\" collises with \"%s\"", tokens[2].c_str(), msg);
            return true;
          }

          return false;
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
      if (m_networkUpdateThread.joinable())
        m_networkUpdateThread.join();

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
    static const std::vector<std::string> VALID_VALUE_NAMES = {
        PhysicsNetworkController::INV_MASS_NAME,        PhysicsNetworkController::POSITION_NAME,
        PhysicsNetworkController::LINEAR_VELOCITY_NAME, PhysicsNetworkController::FORCE_NAME,
        PhysicsNetworkController::ORIENTATION_NAME,     PhysicsNetworkController::ANGULAR_VEL_NAME,
        PhysicsNetworkController::TORQUE_NAME};

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
        char msg = Utility::StringToBool(argv[1], true) ? 'P' : 'R';

        // Broadcast message
        {
          std::lock_guard<std::mutex> lock(m_broker->Mutex());
          m_broker->BroadcastMessage(m_pubSubClients["basic"], "physics/pause", &msg, 1);
        }

        return COMMAND_EXIT_CLEAN;
      };

      physicsCommands->registerCommand(std::make_shared<Command>("pause", func, 2, "Toggles pausing of physics engine."));
    }

    // Get command
    {
      auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
        if (m_broker == nullptr)
        {
          out << "Not connected to a server.\n";
          return 1;
        }

        // Check value name is valid
        if (std::find(VALID_VALUE_NAMES.cbegin(), VALID_VALUE_NAMES.cend(), argv[2]) == VALID_VALUE_NAMES.cend())
        {
          out << "Value name \"" << argv[2] << "\" is not valid!\n";
          return 2;
        }

        // Generate message
        std::string msg = argv[1] + '.' + argv[2];

        // Subscribe physics broker to receive data
        std::string topic = "physics/objects/" + argv[1] + "/" + argv[2];
        m_broker->Subscribe(m_pubSubClients["physics"], topic);

        // Broadcast message
        {
          std::lock_guard<std::mutex> lock(m_broker->Mutex());
          m_broker->BroadcastMessage(m_pubSubClients["physics"], "physics/get", msg.c_str(), (uint16_t)msg.size());
        }

        return COMMAND_EXIT_CLEAN;
      };

      physicsCommands->registerCommand(std::make_shared<Command>("get", func, 3, "Gets a physical value from an object."));
    }

    // Set command
    {
      auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
        if (m_broker == nullptr)
        {
          out << "Not connected to a server.\n";
          return 1;
        }

        std::string valueName = argv[2];
        std::stringstream valueSS(argv[3]);

        // Check value name is valid
        if (std::find(VALID_VALUE_NAMES.cbegin(), VALID_VALUE_NAMES.cend(), valueName) == VALID_VALUE_NAMES.cend())
        {
          out << "Value name \"" << valueName << "\" is not valid!\n";
          return 2;
        }

        // Generate message
        std::string idStr = argv[1] + '.' + valueName + '=';
        uint16_t len = (uint16_t)idStr.size();
        char *msg = nullptr;

        // Get value
        switch (VALUE_TYPES[valueName])
        {
        // Value is float
        case 'F':
        {
          float value;
          valueSS >> value;

          len += sizeof(float);
          msg = new char[len];

          memcpy(msg + idStr.size(), &value, sizeof(float));
          break;
        }
        // Value is 3 element vector
        case '3':
        {
          Vector3 value;
          valueSS >> value;

          len += sizeof(Vector3);
          msg = new char[len];

          memcpy(msg + idStr.size(), &value, sizeof(Vector3));
          break;
        }
        // Value is quaternion
        case 'Q':
        {
          Quaternion value;
          valueSS >> value;

          len += sizeof(Quaternion);
          msg = new char[len];

          memcpy(msg + idStr.size(), &value, sizeof(Quaternion));
          break;
        }
        default:
          return false;
        }

        memcpy(msg, idStr.c_str(), (uint16_t)idStr.size());

        // Broadcast message
        {
          std::lock_guard<std::mutex> lock(m_broker->Mutex());
          m_broker->BroadcastMessage(m_pubSubClients["physics"], "physics/set", msg, len);
        }

        return COMMAND_EXIT_CLEAN;
      };

      physicsCommands->registerCommand(std::make_shared<Command>("set", func, 4, "Sets a physical value."));
    }

    // Subscribe to collisions
    {
      auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
        if (m_broker == nullptr)
        {
          out << "Not connected to a server.\n";
          return 1;
        }

        // Subscribe to responses
        std::string replyTopic = "physics/collision/" + argv[1];
        m_broker->Subscribe(m_pubSubClients["physics"], replyTopic);

        // Broadcast message
        {
          std::lock_guard<std::mutex> lock(m_broker->Mutex());
          m_broker->BroadcastMessage(m_pubSubClients["physics"], "physics/collsub", argv[1].c_str(), (uint16_t) argv[1].size());
        }

        return COMMAND_EXIT_CLEAN;
      };

      physicsCommands->registerCommand(std::make_shared<Command>("collsub", func, 2, "Subscribe to collision messages."));
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
        if (m_pubSubClients["highscores"] == nullptr)
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
        if (m_pubSubClients["savescores"] == nullptr)
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
        if (m_pubSubClients["loadscores"] == nullptr)
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

        // Generate message
        uint16_t len = sizeof(int);
        int value = std::stoi(argv[1]);
        char *msg = new char[len];
        memcpy(msg, &value, len);

        // Broadcast message
        {
          std::lock_guard<std::mutex> lock(m_broker->Mutex());
          m_broker->BroadcastMessage(m_pubSubClients["basic"], "player/ammo_delta", msg, len);
        }

        delete msg;
        return COMMAND_EXIT_CLEAN;
      };

      gameCommands->registerCommand(
          std::make_shared<Command>("ammogive", func, 2, "Gives (or takes away) ammo from the player."));
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
        uint16_t len = sizeof(float);
        float value = std::stof(argv[1]);
        char *msg = new char[len];
        memcpy(msg, &value, len);

        // Broadcast message
        {
          std::lock_guard<std::mutex> lock(m_broker->Mutex());
          m_broker->BroadcastMessage(m_pubSubClients["basic"], "player/shootable_lifetime", msg, len);
        }

        delete msg;
        return COMMAND_EXIT_CLEAN;
      };

      gameCommands->registerCommand(
          std::make_shared<Command>("shootablelife", func, 2, "Sets the lifetime of shootable objects (in seconds)."));
    }

    // Set planet rotation speed
    {
      auto func = [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) -> int {
        if (m_broker == nullptr)
        {
          out << "Not connected to a server.\n";
          return 1;
        }

        // Generate message
        std::string idStr = "planet." + PhysicsNetworkController::ANGULAR_VEL_NAME + '=';
        uint16_t len = (uint16_t)idStr.size() + sizeof(Vector3);
        char *msg = new char[len];
        memcpy(msg, idStr.c_str(), (uint16_t)idStr.size());
        Vector3 value(0.0f, std::stof(argv[1]), 0.0f);
        memcpy(msg + idStr.size(), &value, sizeof(Vector3));

        // Broadcast message
        {
          std::lock_guard<std::mutex> lock(m_broker->Mutex());
          m_broker->BroadcastMessage(m_pubSubClients["physics"], "physics/set", msg, len);
        }

        delete msg;
        return COMMAND_EXIT_CLEAN;
      };

      gameCommands->registerCommand(
          std::make_shared<Command>("planetspeed", func, 2, "Sets the rotational speed of the planet."));
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
