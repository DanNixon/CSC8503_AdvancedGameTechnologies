#include "CourseworkScene.h"

#include <nclgl\Vector4.h>
#include <ncltech\CommonUtils.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\IState.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\SceneManager.h>
#include <ncltech\SortAndSweepBroadphase.h>

#define PLANET_RADIUS 1000.0f

CourseworkScene::CourseworkScene(const std::string &friendlyName)
    : Scene(friendlyName)
    , m_planet(nullptr)
{
  // Debug draw state machine
  {
      // TODO
  }

  // Player state machine
  {
    m_playerStateMachineDefaultState = new IState("idle", m_playerStateMachine.RootState(), &m_playerStateMachine);
    m_playerStateMachineDefaultState->SetActivation(true);

    // Exit conditions
    {
      IState *exitState = new IState("exit", m_playerStateMachine.RootState(), &m_playerStateMachine);
      exitState->AddTransferToTest([]() { return Window::GetKeyboard()->KeyDown(KEYBOARD_X); });
      exitState->AddOnEntryBehaviour([](IState *) {
        NCLDebug::Log("Bye Bye~");
      });
      exitState->AddOnOperateBehaviour([exitState]() {
        if (exitState->TimeInState() > 1.0f)
          SceneManager::Instance()->SetExitFlag(true);
      });
    }

    // Shooting spheres
    {
      IState *shootBall = new IState("shootBall", m_playerStateMachine.RootState(), &m_playerStateMachine);
      shootBall->AddTransferToTest([]() { return Window::GetKeyboard()->KeyDown(KEYBOARD_J); });

      IState *preShoot = new IState("preShoot", shootBall, &m_playerStateMachine);
      shootBall->AddOnEntryBehaviour([preShoot](IState *) { preShoot->SetActivation(true, preShoot->Parent()); });

      IState *shoot = new IState("shoot", shootBall, &m_playerStateMachine);
      shoot->AddTransferToTest([]() { return !Window::GetKeyboard()->KeyDown(KEYBOARD_J); });

      shoot->AddOnEntryBehaviour([](IState *s) {
        float timeHeld = s->TimeInState();
        NCLDebug::Log("TODO: shoot ball here (power %f)", timeHeld);
      });
      shoot->AddTransferFromTest([this]() { return this->m_playerStateMachineDefaultState; });
    }
  }
}

CourseworkScene::~CourseworkScene()
{
}

void CourseworkScene::OnInitializeScene()
{
  PhysicsEngine::Instance()->SetBroadphase(new SortAndSweepBroadphase());

  // Set the camera position
  SceneManager::Instance()->GetCamera()->SetPosition(Vector3(0.0f, PLANET_RADIUS + 50.0f, 0.0f));
  SceneManager::Instance()->GetCamera()->SetYaw(0.0f);
  SceneManager::Instance()->GetCamera()->SetPitch(0.0f);

  // Planet
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
  m_playerStateMachine.Update(dt);
}