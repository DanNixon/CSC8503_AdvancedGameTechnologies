#include "Player.h"

#include <ncltech\NCLDebug.h>
#include <ncltech\SceneManager.h>

#include "ShootableBall.h"
#include "ShootableCube.h"

Player::Player(Scene *scene, PubSubBroker *broker)
    : IPubSubClient(broker)
    , m_scene(scene)
    , m_score(broker)
    , m_shootableLifetime(10.0f)
{
  // Subscribe to topics
  if (broker != nullptr)
  {
    broker->Subscribe(this, "player/ammo_delta");
    broker->Subscribe(this, "player/shootable_lifetime");
  }

  // State machine
  {
    // Idle (default) state
    State *idle = new State("idle", m_playerStateMachine.RootState(), &m_playerStateMachine);

    // Reset state
    State *reset = new State("reset", m_playerStateMachine.RootState(), &m_playerStateMachine);
    m_playerStateMachine.SetDefaultState(reset);
    reset->AddTransferFromTest([idle]() { return idle; });
    reset->AddTransferToTest([]() { return Window::GetKeyboard()->KeyTriggered(PLAYER_RESET_KEY); });
    reset->AddOnEntryBehaviour([this](State *) {
      NCLDebug::Log("Player reset.");

      // Clear shot things
      for (auto it = this->m_shotThings.begin(); it != this->m_shotThings.end(); ++it)
      {
        if (this->m_scene)
          this->m_scene->RemoveGameObject(*it);
      }
      this->m_shotThings.clear();

      // Reset number of shots
      this->m_numShootablesRemaining = 10;

      // Reset score
      this->m_score.Reset();
    });

    // Exit conditions
    {
      State *exitState = new State("exit", m_playerStateMachine.RootState(), &m_playerStateMachine);
      exitState->AddTransferToTest(
          []() { return Window::GetKeyboard()->KeyDown(KEYBOARD_X) || Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE); });
      exitState->AddOnEntryBehaviour([](State *) { NCLDebug::Log("Bye Bye~"); });
      exitState->AddOnOperateBehaviour([exitState]() {
        if (exitState->TimeInState() > 1.0f)
          SceneManager::Instance()->SetExitFlag(true);
      });
    }

    // Shooting
    {
      // End of turn (score submission) state
      State *endOfTurn = new State("end_of_turn", m_playerStateMachine.RootState(), &m_playerStateMachine);
      endOfTurn->AddOnEntryBehaviour([this](State *) {
        if (this->NumShootablesRemaining() == 0)
        {
          NCLDebug::Log("Out of things to shoot! Submitting score...");
          this->m_score.Submit("no name");
          this->m_score.Reset();
        }
      });
      endOfTurn->AddTransferFromTest([idle]() { return idle; });

      State::OnEntryBehaviour powerUpBehaviour = [](State *) { NCLDebug::Log("Hold to power up. Release to fire!"); };
      State::TransferFromTest afterShotTransfer = [endOfTurn]() { return endOfTurn; };

      // Shooting cubes
      {
        State *shootCube = new State("shoot_cube", m_playerStateMachine.RootState(), &m_playerStateMachine);
        shootCube->AddTransferToTest(
            [this]() { return Window::GetKeyboard()->KeyDown(SHOOT_CUBE_KEY) && this->NumShootablesRemaining() > 0; });

        State *preShoot = new State("pre_shoot", shootCube, &m_playerStateMachine);
        preShoot->AddOnEntryBehaviour(powerUpBehaviour);
        shootCube->AddOnEntryBehaviour([this, preShoot](State *) { this->m_playerStateMachine.ActivateState(preShoot); });

        State *shoot = new State("shoot", shootCube, &m_playerStateMachine);
        shoot->AddTransferToTest([]() { return !Window::GetKeyboard()->KeyDown(SHOOT_CUBE_KEY); });
        shoot->AddOnEntryBehaviour([this](State *s) { this->ShootFromCamera(new ShootableCube(this, m_shootableLifetime), s->TimeInState()); });
        shoot->AddTransferFromTest(afterShotTransfer);
      }

      // Shooting spheres
      {
        State *shootBall = new State("shoot_ball", m_playerStateMachine.RootState(), &m_playerStateMachine);
        shootBall->AddTransferToTest(
            [this]() { return Window::GetKeyboard()->KeyDown(SHOOT_BALL_KEY) && this->NumShootablesRemaining() > 0; });

        State *preShoot = new State("pre_shoot", shootBall, &m_playerStateMachine);
        preShoot->AddOnEntryBehaviour(powerUpBehaviour);
        shootBall->AddOnEntryBehaviour([this, preShoot](State *) { this->m_playerStateMachine.ActivateState(preShoot); });

        State *shoot = new State("shoot", shootBall, &m_playerStateMachine);
        shoot->AddTransferToTest([]() { return !Window::GetKeyboard()->KeyDown(SHOOT_BALL_KEY); });
        shoot->AddOnEntryBehaviour([this](State *s) { this->ShootFromCamera(new ShootableBall(this, m_shootableLifetime), s->TimeInState()); });
        shoot->AddTransferFromTest(afterShotTransfer);
      }
    }
  }

  // Set initial state
  Reset();
}

Player::~Player()
{
}

/**
 * @brief Resets player state.
 */
void Player::Reset()
{
  // This should just reset the state machine, resetting other things should be handled in the state machine
  m_playerStateMachine.Reset();
}

/**
 * @brief Updates player state.
 * @param dt TIme in seconds since last update
 */
void Player::Update(float dt)
{
  m_playerStateMachine.Update(dt);

  // Update balls
  for (auto it = m_shotThings.begin(); it != m_shotThings.end();)
  {
    IShootable *b = *it;
    b->GetStateMachine().Update(dt);

    if (b->HasExpired())
    {
      it = m_shotThings.erase(it);
      if (m_scene)
        m_scene->RemoveGameObject(b);
    }
    else
    {
      ++it;
    }
  }
}

/**
 * @copydoc IPubSubCLient::HandleSubscription
 */
bool Player::HandleSubscription(const std::string & topic, const char * msg, uint16_t len)
{
  if (topic == "player/ammo_delta")
  {
    // Offset player ammo
    m_numShootablesRemaining += std::atoi(msg);
  }
  else if (topic == "player/shootable_lifetime")
  {
    // Set shootable lifetime
    memcpy(&m_shootableLifetime, msg, sizeof(float));
  }
  else
  {
    return false;
  }

  return true;
}

/**
 * @brief Shoots a shootable obejct from the camera position in the camera forward direction.
 * @param shootable Object to shoot
 * @param power Velocity scaling (x10)
 */
void Player::ShootFromCamera(IShootable *shootable, float power)
{
  Camera *camera = SceneManager::Instance()->GetCamera();

  Vector3 velocity(0.0f, 0.0f, -power * 10.0f);
  Quaternion::RotatePointByQuaternion(camera->GetOrientation(), velocity);

  shootable->Physics()->SetPosition(camera->GetPosition());
  shootable->Physics()->SetLinearVelocity(velocity);

  m_shotThings.push_back(shootable);
  m_numShootablesRemaining--;

  if (m_scene)
  {
    m_scene->AddGameObject(shootable);

    // Set point gravitation target
    Object *planet = m_scene->FindGameObject("planet");
    if (planet != nullptr && planet->HasPhysics())
      shootable->Physics()->SetGravitationTarget(planet->Physics());
  }
}
