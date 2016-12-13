#include "Player.h"

#include <ncltech\NCLDebug.h>
#include <ncltech\SceneManager.h>

#include "ShootableBall.h"
#include "ShootableCube.h"

Player::Player(Scene *scene)
    : m_scene(scene)
    , m_score(nullptr) // TODO
{
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

      for (auto it = this->m_shotThings.begin(); it != this->m_shotThings.end(); ++it)
      {
        if (this->m_scene)
          this->m_scene->RemoveGameObject(*it);
      }
      this->m_shotThings.clear();
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
      State::OnEntryBehaviour powerUpBehaviour = [](State *) { NCLDebug::Log("Hold to power up. Release to fire!"); };
      State::TransferFromTest returnToIdle = [idle]() { return idle; };

      // Shooting cubes
      {
        State *shootCube = new State("shoot_cube", m_playerStateMachine.RootState(), &m_playerStateMachine);
        shootCube->AddTransferToTest([]() { return Window::GetKeyboard()->KeyDown(SHOOT_CUBE_KEY); });

        State *preShoot = new State("pre_shoot", shootCube, &m_playerStateMachine);
        preShoot->AddOnEntryBehaviour(powerUpBehaviour);
        shootCube->AddOnEntryBehaviour([this, preShoot](State *) { this->m_playerStateMachine.ActivateState(preShoot); });

        State *shoot = new State("shoot", shootCube, &m_playerStateMachine);
        shoot->AddTransferToTest([]() { return !Window::GetKeyboard()->KeyDown(SHOOT_CUBE_KEY); });
        shoot->AddOnEntryBehaviour([this](State *s) { this->ShootFromCamera(new ShootableCube(this), s->TimeInState()); });
        shoot->AddTransferFromTest(returnToIdle);
      }

      // Shooting spheres
      {
        State *shootBall = new State("shoot_ball", m_playerStateMachine.RootState(), &m_playerStateMachine);
        shootBall->AddTransferToTest([]() { return Window::GetKeyboard()->KeyDown(SHOOT_BALL_KEY); });

        State *preShoot = new State("pre_shoot", shootBall, &m_playerStateMachine);
        preShoot->AddOnEntryBehaviour(powerUpBehaviour);
        shootBall->AddOnEntryBehaviour([this, preShoot](State *) { this->m_playerStateMachine.ActivateState(preShoot); });

        State *shoot = new State("shoot", shootBall, &m_playerStateMachine);
        shoot->AddTransferToTest([]() { return !Window::GetKeyboard()->KeyDown(SHOOT_BALL_KEY); });
        shoot->AddOnEntryBehaviour([this](State *s) { this->ShootFromCamera(new ShootableBall(this), s->TimeInState()); });
        shoot->AddTransferFromTest(returnToIdle);
      }
    }

    m_playerStateMachine.Reset();
  }
}

Player::~Player()
{
}

void Player::Reset()
{
  m_playerStateMachine.Reset();
}

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

  if (m_scene)
  {
    m_scene->AddGameObject(shootable);

    // Set point gravitation target
    Object *planet = m_scene->FindGameObject("planet");
    if (planet != nullptr && planet->HasPhysics())
      shootable->Physics()->SetGravitationTarget(planet->Physics());
  }
}