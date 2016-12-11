#include "CourseworkScene.h"

#include <nclgl\OBJMesh.h>
#include <nclgl\Vector4.h>
#include <ncltech\CommonMeshes.h>
#include <ncltech\CommonUtils.h>
#include <ncltech\CuboidCollisionShape.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\ObjectMesh.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\SceneManager.h>
#include <ncltech\SortAndSweepBroadphase.h>
#include <ncltech\SphereCollisionShape.h>
#include <ncltech\State.h>

#define PLANET_RADIUS 250.0f

CourseworkScene::CourseworkScene(const std::string &friendlyName)
    : Scene(friendlyName)
    , m_planetTex(0)
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

  // Load planet texture
  {
    m_planetTex =
        SOIL_load_OGL_texture(TEXTUREDIR "planet.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
                              SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
  }

  // Load target mesh
  {
    m_targetMesh = new OBJMesh(MESHDIR "target.obj");
    m_targetMesh->GenerateNormals();

    GLuint tex =
        SOIL_load_OGL_texture(TEXTUREDIR "target.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
                              SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
    m_targetMesh->SetTexture(tex);
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
  SceneManager::Instance()->GetCamera()->SetPosition(Vector3(0.0f, PLANET_RADIUS + 5.0f, 0.0f));
  SceneManager::Instance()->GetCamera()->SetYaw(0.0f);
  SceneManager::Instance()->GetCamera()->SetPitch(0.0f);

  // Reset state machines
  m_debugDrawStateMachine.Reset();
  m_playerStateMachine.Reset();

  // Create planet
  {
    ObjectMesh *m_planet = new ObjectMesh("planet");

    m_planet->SetMesh(CommonMeshes::Sphere(), false);
    m_planet->SetTexture(m_planetTex, false);
    m_planet->SetColour(Vector4(0.5f, 0.5f, 0.5f, 1.0f));

    m_planet->SetLocalTransform(Matrix4::Scale(Vector3(PLANET_RADIUS, PLANET_RADIUS, PLANET_RADIUS)));
    m_planet->SetBoundingRadius(PLANET_RADIUS);

    m_planet->CreatePhysicsNode();
    m_planet->Physics()->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
    m_planet->Physics()->SetInverseMass(0.0f);

    ICollisionShape *pColshape = new SphereCollisionShape(PLANET_RADIUS);
    m_planet->Physics()->AddCollisionShape(pColshape);
    m_planet->Physics()->SetInverseInertia(pColshape->BuildInverseInertia(0.0f));

    AddGameObject(m_planet);
  }

  // Create target
  {
    ObjectMesh *target = new ObjectMesh("target");
    target->SetMesh(m_targetMesh, false);
    target->CreatePhysicsNode();
    target->Physics()->SetPosition(Vector3(0.0f, PLANET_RADIUS, 0.0f));
    target->Physics()->AddCollisionShape(new CuboidCollisionShape(Vector3(0.5f, 0.5f, 1.0f)));
    target->SetBoundingRadius(10.0f);
    target->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    AddGameObject(target);

    target->Physics()->SetOnCollisionCallback([](PhysicsObject *a, PhysicsObject *b) {
      std::string aname = a->GetAssociatedObject()->GetName();
      std::string bname = b->GetAssociatedObject()->GetName();
      NCLDebug::Log("%s hits %s", aname.c_str(), bname.c_str());
      return true;
    });
  }
}

void CourseworkScene::OnCleanupScene()
{
  Scene::OnCleanupScene();

  // Reset state machines
  m_debugDrawStateMachine.Reset();
  m_playerStateMachine.Reset();

  // Cleanup object pointers
  m_planet = nullptr;
}

void CourseworkScene::OnUpdateScene(float dt)
{
  m_debugDrawStateMachine.Update(dt);
  m_playerStateMachine.Update(dt);
}