#include "PhysicsNetworkController.h"

#include "Object.h"
#include "PhysicsEngine.h"
#include "Utility.h"

const std::string PhysicsNetworkController::INV_MASS_NAME = "inv_mass";
const std::string PhysicsNetworkController::POSITION_NAME = "position";
const std::string PhysicsNetworkController::LINEAR_VELOCITY_NAME = "lin_vel";
const std::string PhysicsNetworkController::FORCE_NAME = "force";
const std::string PhysicsNetworkController::ORIENTATION_NAME = "orientation";
const std::string PhysicsNetworkController::ANGULAR_VEL_NAME = "ang_vel";
const std::string PhysicsNetworkController::TORQUE_NAME = "torque";

PhysicsNetworkController::PhysicsNetworkController(PubSubBroker *broker)
    : IPubSubClient(broker)
{
  // Subscribe to topics
  if (broker != nullptr)
  {
    broker->Subscribe(this, "physics/pause");
    broker->Subscribe(this, "physics/get");
    broker->Subscribe(this, "physics/set");
  }
}

PhysicsNetworkController::~PhysicsNetworkController()
{
}

/**
 * @copydoc IPubSubClient::HandleSubscription
 */
bool PhysicsNetworkController::HandleSubscription(const std::string &topic, const char *msg, uint16_t len)
{
  if (topic == "physics/pause")
  {
    // Handle pause/resume
    PhysicsEngine::Instance()->SetPaused(((char *)msg)[0] == 'P');
  }
  // Get physical property
  else if (topic == "physics/get")
  {
    std::vector<std::string> tokens = Utility::Split(std::string(msg), '.');
    std::string valueName = tokens[1];

    // Find object
    PhysicsObject *obj = PhysicsEngine::Instance()->FindObjectByName(tokens[0]);
    if (obj == nullptr)
      return false;

    // Message
    uint16_t len = 0;
    char *data = nullptr;

    if (valueName == INV_MASS_NAME)
    {
      len = sizeof(float);
      data = new char[len];
      float value = obj->GetInverseMass();
      memcpy(data, &value, len);
    }
    else if (valueName == POSITION_NAME)
    {
      len = sizeof(Vector3);
      data = new char[len];
      Vector3 value = obj->GetPosition();
      memcpy(data, &value, len);
    }
    else if (valueName == LINEAR_VELOCITY_NAME)
    {
      len = sizeof(Vector3);
      data = new char[len];
      Vector3 value = obj->GetLinearVelocity();
      memcpy(data, &value, len);
    }
    else if (valueName == FORCE_NAME)
    {
      len = sizeof(Vector3);
      data = new char[len];
      Vector3 value = obj->GetForce();
      memcpy(data, &value, len);
    }
    else if (valueName == ORIENTATION_NAME)
    {
      len = sizeof(Quaternion);
      data = new char[len];
      Quaternion value = obj->GetOrientation();
      memcpy(data, &value, len);
    }
    else if (valueName == ANGULAR_VEL_NAME)
    {
      len = sizeof(Vector3);
      data = new char[len];
      Vector3 value = obj->GetAngularVelocity();
      memcpy(data, &value, len);
    }
    else if (valueName == TORQUE_NAME)
    {
      len = sizeof(Vector3);
      data = new char[len];
      Vector3 value = obj->GetTorque();
      memcpy(data, &value, len);
    }
    else
    {
      delete data;
      return false;
    }

    // Send message
    std::string topic = "physics/objects/" + tokens[0] + '/' + valueName;
    m_broker->BroadcastMessage(this, topic, data, len);
    delete data;
  }
  // Set physical property
  else if (topic == "physics/set")
  {
    uint16_t idx = 0;
    for (; idx < len; idx++)
    {
      if (msg[idx] == '=')
        break;
    }

    const char *data = msg + idx + 1;
    std::vector<std::string> tokens = Utility::Split(std::string(msg, msg + idx), '.');
    std::string valueName = tokens[1];

    // Find object
    PhysicsObject *obj = PhysicsEngine::Instance()->FindObjectByName(tokens[0]);
    if (obj == nullptr)
      return false;

    if (valueName == INV_MASS_NAME)
    {
      float value;
      memcpy(&value, data, sizeof(value));
      obj->SetInverseMass(value);
    }
    else if (valueName == POSITION_NAME)
    {
      Vector3 value;
      memcpy(&value, data, sizeof(Vector3));
      obj->SetPosition(value);
    }
    else if (valueName == LINEAR_VELOCITY_NAME)
    {
      Vector3 value;
      memcpy(&value, data, sizeof(Vector3));
      obj->SetLinearVelocity(value);
    }
    else if (valueName == FORCE_NAME)
    {
      Vector3 value;
      memcpy(&value, data, sizeof(Vector3));
      obj->SetForce(value);
    }
    else if (valueName == ORIENTATION_NAME)
    {
      Quaternion value;
      memcpy(&value, data, sizeof(Quaternion));
      obj->SetOrientation(value);
    }
    else if (valueName == ANGULAR_VEL_NAME)
    {
      Vector3 value;
      memcpy(&value, data, sizeof(Vector3));
      obj->SetAngularVelocity(value);
    }
    else if (valueName == TORQUE_NAME)
    {
      Vector3 value;
      memcpy(&value, data, sizeof(Vector3));
      obj->SetTorque(value);
    }
    else
    {
      delete data;
      return false;
    }
  }
  // Subscribe to collision detection
  else if (topic == "physics/collsub")
  {
    // TODO
    return false;
  }
  else
  {
    return false;
  }

  return true;
}
