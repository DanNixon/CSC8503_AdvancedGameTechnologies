#include "CourseworkScene.h"

#include <nclgl\Vector4.h>
#include <ncltech\CommonUtils.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\SceneManager.h>
#include <ncltech\SortAndSweepBroadphase.h>
#include <ncltech\State.h>

#define PLANET_RADIUS 100.0f

CourseworkScene::CourseworkScene(const std::string &friendlyName)
    : Scene(friendlyName)
    , m_planet(nullptr)
{
  // Debug draw state machine
  {
    const KeyboardKeys PHYSICS_DEBUG_VIEW_KEY = KeyboardKeys::KEYBOARD_M;

    State *normal = new State("normal", m_debugDrawStateMachine.RootState(), &m_debugDrawStateMachine);
    normal->AddOnEntryBehaviour([](State *) { NCLDebug::Log("Physics view: normal"); });
    // TODO

    State *view1 = new State("view1", m_debugDrawStateMachine.RootState(), &m_debugDrawStateMachine);
    view1->AddOnEntryBehaviour([](State *) { NCLDebug::Log("Physics view: view1"); });
    // TODO

    State *view2 = new State("view2", m_debugDrawStateMachine.RootState(), &m_debugDrawStateMachine);
    view2->AddOnEntryBehaviour([](State *) { NCLDebug::Log("Physics view: view2"); });
    // TODO

    // State transfers
    normal->AddTransferFromTest([PHYSICS_DEBUG_VIEW_KEY, view1]() -> State * {
      return Window::GetKeyboard()->KeyTriggered(PHYSICS_DEBUG_VIEW_KEY) ? view1 : nullptr;
    });

    view1->AddTransferFromTest([PHYSICS_DEBUG_VIEW_KEY, view2]() -> State * {
      return Window::GetKeyboard()->KeyTriggered(PHYSICS_DEBUG_VIEW_KEY) ? view2 : nullptr;
    });

    view2->AddTransferFromTest([PHYSICS_DEBUG_VIEW_KEY, normal]() -> State * {
      return Window::GetKeyboard()->KeyTriggered(PHYSICS_DEBUG_VIEW_KEY) ? normal : nullptr;
    });

    // Default state
    m_debugDrawStateMachine.SetDefaultState(normal);
  }

  // Player state machine
  {
    // Idle (default) state
    State *idle = new State("idle", m_playerStateMachine.RootState(), &m_playerStateMachine);

    // Reset state
    State *reset = new State("reset", m_playerStateMachine.RootState(), &m_playerStateMachine);
    m_playerStateMachine.SetDefaultState(reset);
    reset->AddTransferFromTest([idle]() { return idle; });
    reset->AddOnEntryBehaviour([](State *) { NCLDebug::Log("TODO: clear balls"); });

    // Exit conditions
    {
      State *exitState = new State("exit", m_playerStateMachine.RootState(), &m_playerStateMachine);
      exitState->AddTransferToTest([]() { return Window::GetKeyboard()->KeyDown(KEYBOARD_X); });
      exitState->AddOnEntryBehaviour([](State *) { NCLDebug::Log("Bye Bye~"); });
      exitState->AddOnOperateBehaviour([exitState]() {
        if (exitState->TimeInState() > 1.0f)
          SceneManager::Instance()->SetExitFlag(true);
      });
    }

    // Shooting spheres
    {
      State *shootBall = new State("shootBall", m_playerStateMachine.RootState(), &m_playerStateMachine);
      shootBall->AddTransferToTest([]() { return Window::GetKeyboard()->KeyDown(KEYBOARD_J); });

      State *preShoot = new State("preShoot", shootBall, &m_playerStateMachine);
      preShoot->AddOnEntryBehaviour([](State *) {
        NCLDebug::Log("Hold J to power up.");
        NCLDebug::Log("Release J to fire ball!");
      });
      shootBall->AddOnEntryBehaviour([preShoot](State *) { preShoot->SetActivation(true, preShoot->Parent()); });

      State *shoot = new State("shoot", shootBall, &m_playerStateMachine);
      shoot->AddTransferToTest([]() { return !Window::GetKeyboard()->KeyDown(KEYBOARD_J); });

      shoot->AddOnEntryBehaviour([this](State *s) {
        Camera *camera = SceneManager::Instance()->GetCamera();

        Vector3 velocity(0.0f, 0.0f, -s->TimeInState() * 10.0f);
        Quaternion::RotatePointByQuaternion(camera->GetOrientation(), velocity);

        Object *sphere = CommonUtils::BuildSphereObject("player_shot_sphere", camera->GetPosition(), 1.0f, true, 1.0f, true,
                                                        false, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
        sphere->Physics()->SetLinearVelocity(velocity);
        this->m_shotSpheres.push(sphere);
        this->AddGameObject(sphere);

        NCLDebug::Log("Shot ball with velocity (%f, %f, %f)", velocity.x, velocity.y, velocity.z);
      });
      shoot->AddTransferFromTest([idle]() { return idle; });
    }
  }
}

CourseworkScene::~CourseworkScene()
{
}

void CourseworkScene::OnInitializeScene()
{
  // Set broadphase method
  PhysicsEngine::Instance()->SetBroadphase(new SortAndSweepBroadphase());

  // Set the camera position
  SceneManager::Instance()->GetCamera()->SetPosition(Vector3(0.0f, PLANET_RADIUS + 50.0f, 0.0f));
  SceneManager::Instance()->GetCamera()->SetYaw(0.0f);
  SceneManager::Instance()->GetCamera()->SetPitch(0.0f);

  // Reset state machines
  m_debugDrawStateMachine.Reset();
  m_playerStateMachine.Reset();

  // Planet object
  m_planet = CommonUtils::BuildSphereObject("planet", Vector3(0.0f, 0.0f, 0.0f), PLANET_RADIUS, true, 0.0f, true, false,
                                            Vector4(0.2f, 0.5f, 1.0f, 1.0f));
  AddGameObject(m_planet);
}

void CourseworkScene::OnCleanupScene()
{
  Scene::OnCleanupScene();

  m_planet = nullptr;
}

void CourseworkScene::OnUpdateScene(float dt)
{
  m_debugDrawStateMachine.Update(dt);
  m_playerStateMachine.Update(dt);
}