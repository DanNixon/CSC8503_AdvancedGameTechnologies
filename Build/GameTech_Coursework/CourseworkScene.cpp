#include "CourseworkScene.h"

#include <nclgl\OBJMesh.h>
#include <nclgl\Vector4.h>
#include <ncltech\BruteForceBroadphase.h>
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
#include <ncltech\WeldConstraint.h>

#define PLANET_RADIUS 250.0f

CourseworkScene::CourseworkScene(const std::string &friendlyName)
    : Scene(friendlyName)
    , m_planetTex(0)
    , m_planet(nullptr)
{
  // Debug draw state machine
  {
    const KeyboardKeys PHYSICS_DEBUG_VIEW_KEY = KeyboardKeys::KEYBOARD_M;

    State *viewNormal = new State("normal", m_debugDrawStateMachine.RootState(), &m_debugDrawStateMachine);
    viewNormal->AddOnEntryBehaviour([](State *) {
      NCLDebug::Log("Physics view: off");
      PhysicsEngine::Instance()->SetDebugDrawFlags(0);
    });

    State *viewLinearMotion = new State("linear_motion", m_debugDrawStateMachine.RootState(), &m_debugDrawStateMachine);
    viewLinearMotion->AddOnEntryBehaviour([](State *) {
      NCLDebug::Log("Physics view: linear motion");
      PhysicsEngine::Instance()->SetDebugDrawFlags(DEBUGDRAW_FLAGS_LINEARFORCE | DEBUGDRAW_FLAGS_LINEARVELOCITY);
    });

    State *viewBoundingVolumes = new State("bounding_volumes", m_debugDrawStateMachine.RootState(), &m_debugDrawStateMachine);
    viewBoundingVolumes->AddOnEntryBehaviour([](State *) {
      NCLDebug::Log("Physics view: bounding volumes");
      PhysicsEngine::Instance()->SetDebugDrawFlags(DEBUGDRAW_FLAGS_AABB);
    });

    State *viewBroadphase = new State("broadphase", m_debugDrawStateMachine.RootState(), &m_debugDrawStateMachine);
    viewBroadphase->AddOnEntryBehaviour([](State *) {
      NCLDebug::Log("Physics view: broadphase");
      PhysicsEngine::Instance()->SetDebugDrawFlags(DEBUGDRAW_FLAGS_BROADPHASE | DEBUGDRAW_FLAGS_BROADPHASE_PAIRS);
    });

    State *viewCollisions = new State("collisions", m_debugDrawStateMachine.RootState(), &m_debugDrawStateMachine);
    viewCollisions->AddOnEntryBehaviour([](State *) {
      NCLDebug::Log("Physics view: collisions");
      PhysicsEngine::Instance()->SetDebugDrawFlags(DEBUGDRAW_FLAGS_COLLISIONVOLUMES | DEBUGDRAW_FLAGS_COLLISIONNORMALS |
                                                   DEBUGDRAW_FLAGS_MANIFOLD);
    });

    State *viewConstraints = new State("constraints", m_debugDrawStateMachine.RootState(), &m_debugDrawStateMachine);
    viewConstraints->AddOnEntryBehaviour([](State *) {
      NCLDebug::Log("Physics view: constraints");
      PhysicsEngine::Instance()->SetDebugDrawFlags(DEBUGDRAW_FLAGS_CONSTRAINT);
    });

    // State transfers
    viewNormal->AddTransferFromTest([PHYSICS_DEBUG_VIEW_KEY, viewLinearMotion]() -> State * {
      return Window::GetKeyboard()->KeyTriggered(PHYSICS_DEBUG_VIEW_KEY) ? viewLinearMotion : nullptr;
    });

    viewLinearMotion->AddTransferFromTest([PHYSICS_DEBUG_VIEW_KEY, viewBoundingVolumes]() -> State * {
      return Window::GetKeyboard()->KeyTriggered(PHYSICS_DEBUG_VIEW_KEY) ? viewBoundingVolumes : nullptr;
    });

    viewBoundingVolumes->AddTransferFromTest([PHYSICS_DEBUG_VIEW_KEY, viewBroadphase]() -> State * {
      return Window::GetKeyboard()->KeyTriggered(PHYSICS_DEBUG_VIEW_KEY) ? viewBroadphase : nullptr;
    });

    viewBroadphase->AddTransferFromTest([PHYSICS_DEBUG_VIEW_KEY, viewCollisions]() -> State * {
      return Window::GetKeyboard()->KeyTriggered(PHYSICS_DEBUG_VIEW_KEY) ? viewCollisions : nullptr;
    });

    viewCollisions->AddTransferFromTest([PHYSICS_DEBUG_VIEW_KEY, viewConstraints]() -> State * {
      return Window::GetKeyboard()->KeyTriggered(PHYSICS_DEBUG_VIEW_KEY) ? viewConstraints : nullptr;
    });

    viewConstraints->AddTransferFromTest([PHYSICS_DEBUG_VIEW_KEY, viewNormal]() -> State * {
      return Window::GetKeyboard()->KeyTriggered(PHYSICS_DEBUG_VIEW_KEY) ? viewNormal : nullptr;
    });

    // Default state
    m_debugDrawStateMachine.SetDefaultState(viewNormal);
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
        sphere->Physics()->SetGravitationTarget(this->m_planet->Physics());
        sphere->Physics()->AutoResizeBoundingBox();
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
  PhysicsEngine::Instance()->SetGravity(Vector3(0.0f, 0.0f, 0.0f));

  // Set the camera position
  SceneManager::Instance()->GetCamera()->SetPosition(Vector3(PLANET_RADIUS + 5.0f, 0.0f, 0.0f));
  SceneManager::Instance()->GetCamera()->SetYaw(0.0f);
  SceneManager::Instance()->GetCamera()->SetPitch(0.0f);

  // Reset state machines
  m_debugDrawStateMachine.Reset();
  m_playerStateMachine.Reset();

  // Create planet
  {
    m_planet = new ObjectMesh("planet");

    m_planet->SetMesh(CommonMeshes::Sphere(), false);
    m_planet->SetTexture(m_planetTex, false);
    m_planet->SetColour(Vector4(0.5f, 0.5f, 0.5f, 1.0f));

    m_planet->SetLocalTransform(Matrix4::Scale(Vector3(PLANET_RADIUS, PLANET_RADIUS, PLANET_RADIUS)));
    m_planet->SetBoundingRadius(PLANET_RADIUS);

    m_planet->CreatePhysicsNode();
    m_planet->Physics()->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
    m_planet->Physics()->SetInverseMass(0.0f);

    ICollisionShape *shape = new SphereCollisionShape(PLANET_RADIUS);
    m_planet->Physics()->AddCollisionShape(shape);
    m_planet->Physics()->SetInverseInertia(shape->BuildInverseInertia(0.0f));

    m_planet->Physics()->AutoResizeBoundingBox();

    AddGameObject(m_planet);
  }

  // Create target
  {
    m_target = new ObjectMesh("target");

    m_target->SetMesh(m_targetMesh, false);
    m_target->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    m_target->SetBoundingRadius(10.0f);

    m_target->CreatePhysicsNode();
    m_target->Physics()->SetPosition(Vector3(PLANET_RADIUS, 0.0f, 0.0f));
    m_target->Physics()->SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0.0f, 0.0f, 1.0f), 90.0f));

    m_target->Physics()->SetInverseMass(0.0f);

    ICollisionShape *shape = new CuboidCollisionShape(Vector3(0.5f, 0.5f, 1.0f)); // TODO
    m_target->Physics()->AddCollisionShape(shape);
    m_target->Physics()->SetInverseInertia(shape->BuildInverseInertia(0.0f));

    m_target->Physics()->AutoResizeBoundingBox();

    AddGameObject(m_target);

    m_target->Physics()->SetOnCollisionCallback([this](PhysicsObject *a, PhysicsObject *b) {
      bool handle = b != this->m_planet->Physics();

      if (handle)
      {
        std::string aname = a->GetAssociatedObject()->GetName();
        std::string bname = b->GetAssociatedObject()->GetName();
        NCLDebug::Log("%s hits %s", aname.c_str(), bname.c_str());
      }

      return handle;
    });

    // Fix target position to planet
    // TODO
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
  // Update state machines
  m_debugDrawStateMachine.Update(dt);
  m_playerStateMachine.Update(dt);

  // Add planet rotation
  m_planet->Physics()->SetAngularVelocity(Vector3(0.0f, 0.05f, 0.0f));
}