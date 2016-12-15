#include "CourseworkScene.h"

#include <nclgl\OBJMesh.h>
#include <nclgl\Vector4.h>
#include <ncltech\BruteForceBroadphase.h>
#include <ncltech\CommonMeshes.h>
#include <ncltech\CommonUtils.h>
#include <ncltech\CuboidCollisionShape.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\HullCollisionShape.h>
#include <ncltech\ObjectMesh.h>
#include <ncltech\OctreeBroadphase.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\SceneManager.h>
#include <ncltech\SortAndSweepBroadphase.h>
#include <ncltech\SphereCollisionShape.h>
#include <ncltech\State.h>
#include <ncltech\WeldConstraint.h>

#define CLIENT_PORT 1000
#define SERVER_PORT 1200

/**
 * @brief Radius of the planet.
 */
const float CourseworkScene::PLANET_RADIUS = 50.0f;

/**
 * @brief Prints the key mappings to the log.
 */
void CourseworkScene::PrintKeyMapping()
{
  NCLDebug::Log("Key mappings:");
  NCLDebug::Log("  Reset scene: %c", KeyboardKeys::KEYBOARD_R);
  NCLDebug::Log("  Reset player: %c", Player::PLAYER_RESET_KEY);
  NCLDebug::Log("  Shoot cube: %c", Player::SHOOT_CUBE_KEY);
  NCLDebug::Log("  Shoot ball: %c", Player::SHOOT_BALL_KEY);
  NCLDebug::Log("  Switch physics view mode: %c", PHYSICS_DEBUG_VIEW_KEY);
  NCLDebug::Log("  Switch broadphase culling mode: %c", BROADPHASE_MODE_KEY);
  NCLDebug::Log("  Switch integrations mode: %c", INTEGRATION_MODE_KEY);
  NCLDebug::Log("  Toggle atmosphere: %c", ATMOSPHERE_TOGGLE_KEY);
}

/**
 * @brief Creates a new courseowrk scene.
 */
CourseworkScene::CourseworkScene()
    : Scene("GameTech coursework")
    , m_broker(nullptr)
    , m_netAnnounceClient(nullptr)
    , m_physicsNetClient(nullptr)
    , m_planetTex(0)
    , m_targetMesh(nullptr)
    , m_lampPostMesh(nullptr)
    , m_planet(nullptr)
    , m_atmosphere(nullptr)
    , m_target(nullptr)
    , m_lampPost(nullptr)
    , m_spaceCube(nullptr)
    , m_spaceSphere(nullptr)
    , m_testQuad1(nullptr)
    , m_testQuad2(nullptr)
    , m_softBody(nullptr)
{
  // Debug draw state machine (overfill for a state machine really...)
  {
    const Vector4 PHYSICS_VIEW_STATUS_COLOUR(1.0f, 1.0f, 0.0f, 1.0f);

    // States
    State *viewNormal = new State("normal", m_debugDrawStateMachine.RootState(), &m_debugDrawStateMachine);
    viewNormal->AddOnEntryBehaviour([](State *) { PhysicsEngine::Instance()->SetDebugDrawFlags(0); });
    viewNormal->AddOnOperateBehaviour(
        [PHYSICS_VIEW_STATUS_COLOUR]() { NCLDebug::AddStatusEntry(PHYSICS_VIEW_STATUS_COLOUR, "Physics view: off"); });

    State *viewLinearMotion = new State("linear_motion", m_debugDrawStateMachine.RootState(), &m_debugDrawStateMachine);
    viewLinearMotion->AddOnEntryBehaviour([](State *) {
      PhysicsEngine::Instance()->SetDebugDrawFlags(DEBUGDRAW_FLAGS_LINEARFORCE | DEBUGDRAW_FLAGS_LINEARVELOCITY);
    });
    viewLinearMotion->AddOnOperateBehaviour(
        [PHYSICS_VIEW_STATUS_COLOUR]() { NCLDebug::AddStatusEntry(PHYSICS_VIEW_STATUS_COLOUR, "Physics view: linear motion"); });

    State *viewBoundingVolumes = new State("bounding_volumes", m_debugDrawStateMachine.RootState(), &m_debugDrawStateMachine);
    viewBoundingVolumes->AddOnEntryBehaviour([](State *) { PhysicsEngine::Instance()->SetDebugDrawFlags(DEBUGDRAW_FLAGS_AABB); });
    viewBoundingVolumes->AddOnOperateBehaviour(
        [PHYSICS_VIEW_STATUS_COLOUR]() { NCLDebug::AddStatusEntry(PHYSICS_VIEW_STATUS_COLOUR, "Physics view: AABB"); });

    State *viewBroadphase = new State("broadphase", m_debugDrawStateMachine.RootState(), &m_debugDrawStateMachine);
    viewBroadphase->AddOnEntryBehaviour([](State *) {
      PhysicsEngine::Instance()->SetDebugDrawFlags(DEBUGDRAW_FLAGS_BROADPHASE | DEBUGDRAW_FLAGS_BROADPHASE_PAIRS);
    });
    viewBroadphase->AddOnOperateBehaviour(
        [PHYSICS_VIEW_STATUS_COLOUR]() { NCLDebug::AddStatusEntry(PHYSICS_VIEW_STATUS_COLOUR, "Physics view: broadphase"); });

    State *viewCollisions = new State("collisions", m_debugDrawStateMachine.RootState(), &m_debugDrawStateMachine);
    viewCollisions->AddOnEntryBehaviour([](State *) {
      PhysicsEngine::Instance()->SetDebugDrawFlags(DEBUGDRAW_FLAGS_COLLISIONVOLUMES | DEBUGDRAW_FLAGS_COLLISIONNORMALS |
                                                   DEBUGDRAW_FLAGS_MANIFOLD);
    });
    viewCollisions->AddOnOperateBehaviour(
        [PHYSICS_VIEW_STATUS_COLOUR]() { NCLDebug::AddStatusEntry(PHYSICS_VIEW_STATUS_COLOUR, "Physics view: collisions"); });

    State *viewConstraints = new State("constraints", m_debugDrawStateMachine.RootState(), &m_debugDrawStateMachine);
    viewConstraints->AddOnEntryBehaviour(
        [](State *) { PhysicsEngine::Instance()->SetDebugDrawFlags(DEBUGDRAW_FLAGS_CONSTRAINT); });
    viewConstraints->AddOnOperateBehaviour(
        [PHYSICS_VIEW_STATUS_COLOUR]() { NCLDebug::AddStatusEntry(PHYSICS_VIEW_STATUS_COLOUR, "Physics view: constraints"); });

    // State transitions
    viewNormal->AddTransferFromTest([viewLinearMotion]() {
      return Window::GetKeyboard()->KeyTriggered(PHYSICS_DEBUG_VIEW_KEY) ? viewLinearMotion : nullptr;
    });

    viewLinearMotion->AddTransferFromTest([viewBoundingVolumes]() {
      return Window::GetKeyboard()->KeyTriggered(PHYSICS_DEBUG_VIEW_KEY) ? viewBoundingVolumes : nullptr;
    });

    viewBoundingVolumes->AddTransferFromTest(
        [viewBroadphase]() { return Window::GetKeyboard()->KeyTriggered(PHYSICS_DEBUG_VIEW_KEY) ? viewBroadphase : nullptr; });

    viewBroadphase->AddTransferFromTest(
        [viewCollisions]() { return Window::GetKeyboard()->KeyTriggered(PHYSICS_DEBUG_VIEW_KEY) ? viewCollisions : nullptr; });

    viewCollisions->AddTransferFromTest(
        [viewConstraints]() { return Window::GetKeyboard()->KeyTriggered(PHYSICS_DEBUG_VIEW_KEY) ? viewConstraints : nullptr; });

    viewConstraints->AddTransferFromTest(
        [viewNormal]() -> State * { return Window::GetKeyboard()->KeyTriggered(PHYSICS_DEBUG_VIEW_KEY) ? viewNormal : nullptr; });

    // Default state
    m_debugDrawStateMachine.SetDefaultState(viewNormal);
  }

  // Broadphase mode state machine (again, overkill)
  {
    const Vector4 BROADPHASE_MODE_STATUS_COLOUR(0.0f, 1.0f, 1.0f, 1.0f);

    State::OnEntryBehaviour removePrevBroadphase = [](State *) {
      delete PhysicsEngine::Instance()->GetBroadphase();
      PhysicsEngine::Instance()->SetBroadphase(nullptr);
    };

    // States
    State *sortAndSweepX = new State("sort_and_sweep_x", m_broadphaseModeStateMachine.RootState(), &m_broadphaseModeStateMachine);
    sortAndSweepX->AddOnEntryBehaviour(removePrevBroadphase);
    sortAndSweepX->AddOnEntryBehaviour(
        [](State *) { PhysicsEngine::Instance()->SetBroadphase(new SortAndSweepBroadphase(Vector3(1.0f, 0.0f, 0.0f))); });
    sortAndSweepX->AddOnOperateBehaviour([BROADPHASE_MODE_STATUS_COLOUR]() {
      NCLDebug::AddStatusEntry(BROADPHASE_MODE_STATUS_COLOUR, "Broadphase: sort and sweep (x axis)");
    });

    State *sortAndSweepY = new State("sort_and_sweep_y", m_broadphaseModeStateMachine.RootState(), &m_broadphaseModeStateMachine);
    sortAndSweepY->AddOnEntryBehaviour(removePrevBroadphase);
    sortAndSweepY->AddOnEntryBehaviour(
        [](State *) { PhysicsEngine::Instance()->SetBroadphase(new SortAndSweepBroadphase(Vector3(0.0f, 1.0f, 0.0f))); });
    sortAndSweepY->AddOnOperateBehaviour([BROADPHASE_MODE_STATUS_COLOUR]() {
      NCLDebug::AddStatusEntry(BROADPHASE_MODE_STATUS_COLOUR, "Broadphase: sort and sweep (y axis)");
    });

    State *sortAndSweepZ = new State("sort_and_sweep_z", m_broadphaseModeStateMachine.RootState(), &m_broadphaseModeStateMachine);
    sortAndSweepZ->AddOnEntryBehaviour(removePrevBroadphase);
    sortAndSweepZ->AddOnEntryBehaviour(
        [](State *) { PhysicsEngine::Instance()->SetBroadphase(new SortAndSweepBroadphase(Vector3(0.0f, 0.0f, 1.0f))); });
    sortAndSweepZ->AddOnOperateBehaviour([BROADPHASE_MODE_STATUS_COLOUR]() {
      NCLDebug::AddStatusEntry(BROADPHASE_MODE_STATUS_COLOUR, "Broadphase: sort and sweep (z axis)");
    });

    State *bruteForce = new State("brute_force", m_broadphaseModeStateMachine.RootState(), &m_broadphaseModeStateMachine);
    bruteForce->AddOnEntryBehaviour(removePrevBroadphase);
    bruteForce->AddOnEntryBehaviour([](State *) { PhysicsEngine::Instance()->SetBroadphase(new BruteForceBroadphase()); });
    bruteForce->AddOnOperateBehaviour([BROADPHASE_MODE_STATUS_COLOUR]() {
      NCLDebug::AddStatusEntry(BROADPHASE_MODE_STATUS_COLOUR, "Broadphase: off (brute force)");
    });

    State *octree = new State("octree", m_broadphaseModeStateMachine.RootState(), &m_broadphaseModeStateMachine);
    octree->AddOnEntryBehaviour(removePrevBroadphase);
    octree->AddOnEntryBehaviour(
        [](State *) { PhysicsEngine::Instance()->SetBroadphase(new OctreeBroadphase(10, 4, new BruteForceBroadphase())); });
    octree->AddOnOperateBehaviour([BROADPHASE_MODE_STATUS_COLOUR]() {
      NCLDebug::AddStatusEntry(BROADPHASE_MODE_STATUS_COLOUR, "Broadphase: octree (obj. lim 10, div. lim. 4, brute force)");
    });

    State *octreeSortSweepHybrid =
        new State("octree_sortsweep_hybrid", m_broadphaseModeStateMachine.RootState(), &m_broadphaseModeStateMachine);
    octreeSortSweepHybrid->AddOnEntryBehaviour(removePrevBroadphase);
    octreeSortSweepHybrid->AddOnEntryBehaviour(
        [](State *) { PhysicsEngine::Instance()->SetBroadphase(new OctreeBroadphase(10, 4, new SortAndSweepBroadphase())); });
    octreeSortSweepHybrid->AddOnOperateBehaviour([BROADPHASE_MODE_STATUS_COLOUR]() {
      NCLDebug::AddStatusEntry(BROADPHASE_MODE_STATUS_COLOUR, "Broadphase: octree (obj. lim 10, div. lim. 4, sort/sweep)");
    });

    // State transitions
    sortAndSweepX->AddTransferFromTest(
        [sortAndSweepY]() { return Window::GetKeyboard()->KeyTriggered(BROADPHASE_MODE_KEY) ? sortAndSweepY : nullptr; });

    sortAndSweepY->AddTransferFromTest(
        [sortAndSweepZ]() { return Window::GetKeyboard()->KeyTriggered(BROADPHASE_MODE_KEY) ? sortAndSweepZ : nullptr; });

    sortAndSweepZ->AddTransferFromTest(
        [bruteForce]() { return Window::GetKeyboard()->KeyTriggered(BROADPHASE_MODE_KEY) ? bruteForce : nullptr; });

    bruteForce->AddTransferFromTest(
        [octree]() { return Window::GetKeyboard()->KeyTriggered(BROADPHASE_MODE_KEY) ? octree : nullptr; });

    octree->AddTransferFromTest([octreeSortSweepHybrid]() {
      return Window::GetKeyboard()->KeyTriggered(BROADPHASE_MODE_KEY) ? octreeSortSweepHybrid : nullptr;
    });

    octreeSortSweepHybrid->AddTransferFromTest(
        [sortAndSweepX]() { return Window::GetKeyboard()->KeyTriggered(BROADPHASE_MODE_KEY) ? sortAndSweepX : nullptr; });

    // Default state
    m_broadphaseModeStateMachine.SetDefaultState(sortAndSweepX);
  }

  // Integration mode state machine (such overkill...)
  {
    const Vector4 INTEGRATION_MODE_STATUS_COLOUR(1.0f, 0.0f, 1.0f, 1.0f);

    // States
    State *explicitEuler = new State("explicit_euler", m_integrationModeStateMachine.RootState(), &m_integrationModeStateMachine);
    explicitEuler->AddOnEntryBehaviour(
        [](State *) { PhysicsEngine::Instance()->SetIntegrationType(INTEGRATION_EXPLICIT_EULER); });
    explicitEuler->AddOnOperateBehaviour([INTEGRATION_MODE_STATUS_COLOUR]() {
      NCLDebug::AddStatusEntry(INTEGRATION_MODE_STATUS_COLOUR, "Integration: explicit Euler");
    });

    State *semiImplicitEuler =
        new State("semi_implicit_euler", m_integrationModeStateMachine.RootState(), &m_integrationModeStateMachine);
    semiImplicitEuler->AddOnEntryBehaviour(
        [](State *) { PhysicsEngine::Instance()->SetIntegrationType(INTEGRATION_SEMI_IMPLICIT_EULER); });
    semiImplicitEuler->AddOnOperateBehaviour([INTEGRATION_MODE_STATUS_COLOUR]() {
      NCLDebug::AddStatusEntry(INTEGRATION_MODE_STATUS_COLOUR, "Integration: semi-implicit Euler");
    });

    State *rk2 = new State("rk2", m_integrationModeStateMachine.RootState(), &m_integrationModeStateMachine);
    rk2->AddOnEntryBehaviour([](State *) { PhysicsEngine::Instance()->SetIntegrationType(INTEGRATION_RUNGE_KUTTA_2); });
    rk2->AddOnOperateBehaviour(
        [INTEGRATION_MODE_STATUS_COLOUR]() { NCLDebug::AddStatusEntry(INTEGRATION_MODE_STATUS_COLOUR, "Integration: RK2"); });

    State *rk4 = new State("rk4", m_integrationModeStateMachine.RootState(), &m_integrationModeStateMachine);
    rk4->AddOnEntryBehaviour([](State *) { PhysicsEngine::Instance()->SetIntegrationType(INTEGRATION_RUNGE_KUTTA_4); });
    rk4->AddOnOperateBehaviour(
        [INTEGRATION_MODE_STATUS_COLOUR]() { NCLDebug::AddStatusEntry(INTEGRATION_MODE_STATUS_COLOUR, "Integration: RK4"); });

    // State transitions
    explicitEuler->AddTransferFromTest([semiImplicitEuler]() {
      return Window::GetKeyboard()->KeyTriggered(INTEGRATION_MODE_KEY) ? semiImplicitEuler : nullptr;
    });

    semiImplicitEuler->AddTransferFromTest(
        [rk2]() { return Window::GetKeyboard()->KeyTriggered(INTEGRATION_MODE_KEY) ? rk2 : nullptr; });

    rk2->AddTransferFromTest([rk4]() { return Window::GetKeyboard()->KeyTriggered(INTEGRATION_MODE_KEY) ? rk4 : nullptr; });

    rk4->AddTransferFromTest(
        [explicitEuler]() { return Window::GetKeyboard()->KeyTriggered(INTEGRATION_MODE_KEY) ? explicitEuler : nullptr; });

    // Default state
    m_integrationModeStateMachine.SetDefaultState(semiImplicitEuler);
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

  // Load lamp post mesh
  {
    m_lampPostMesh = new OBJMesh(MESHDIR "lamp_post.obj");
    m_lampPostMesh->GenerateNormals();
  }
}

CourseworkScene::~CourseworkScene()
{
  delete m_netAnnounceClient;
  delete m_physicsNetClient;
}

void CourseworkScene::OnInitializeScene()
{
  // Connect to server
  m_broker = new PubSubBrokerNetNode();
  if (m_broker->Initialize(CLIENT_PORT, 8))
  {
    const uint8_t SERVER_IP[] = {127, 0, 0, 1};
    m_broker->ConnectToBroker(SERVER_IP, SERVER_PORT);

    // Add network announcement client
    m_netAnnounceClient = new FunctionalPubSubClient(m_broker);
    m_broker->Subscribe(m_netAnnounceClient, "announce");
    m_netAnnounceClient->SetSubscriptionHandler([&](const std::string &topic, const char *msg, uint16_t) {
      NCLDebug::Log("[ANNOUNCE] %s\n", msg);
      return true;
    });

    // Add physics control client
    m_physicsNetClient = new PhysicsNetworkController(m_broker);

    // Start network update thread
    m_networkThread = std::thread(&CourseworkScene::BrokerNetworkLoop, this);
  }
  else
  {
    NCLERROR("Failed to init networking!");
  }

  // Show key mappings
  PrintKeyMapping();

  // Configure physics
  PhysicsEngine::Instance()->SetBroadphase(new SortAndSweepBroadphase());
  PhysicsEngine::Instance()->SetGravity(Vector3(0.0f, 0.0f, 0.0f));

  // Set the camera position
  SceneManager::Instance()->GetCamera()->SetPosition(Vector3(PLANET_RADIUS + 5.0f, 0.0f, 0.0f));
  SceneManager::Instance()->GetCamera()->SetYaw(0.0f);
  SceneManager::Instance()->GetCamera()->SetPitch(0.0f);

  // Reset state machines
  m_debugDrawStateMachine.Reset();
  m_broadphaseModeStateMachine.Reset();
  m_integrationModeStateMachine.Reset();

  // Create planet
  {
    const Vector3 PLANET_POSITION(0.0f, 0.0f, 0.0f);

    m_planet = new ObjectMesh("planet");

    m_planet->SetMesh(CommonMeshes::Sphere(), false);
    m_planet->SetTexture(m_planetTex, false);

    m_planet->SetLocalTransform(Matrix4::Scale(PLANET_RADIUS));

    m_planet->CreatePhysicsNode();
    m_planet->Physics()->SetPosition(PLANET_POSITION);
    m_planet->Physics()->SetInverseMass(0.0f);
    m_planet->Physics()->SetFriction(0.8f);
    m_planet->Physics()->SetElasticity(0.5f);

    ICollisionShape *shape = new SphereCollisionShape(PLANET_RADIUS);
    m_planet->Physics()->AddCollisionShape(shape);
    m_planet->Physics()->SetInverseInertia(shape->BuildInverseInertia(0.0f));

    // Never perform at rest test on planet (should always rotate)
    m_planet->Physics()->SetRestVelocityThreshold(0.0f);

    m_planet->Physics()->AutoResizeBoundingBox();

    AddGameObject(m_planet);

    // Planet rotation
    m_planet->Physics()->SetDampingCoefficient(1.0f);
    m_planet->Physics()->SetAngularVelocity(Vector3(0.0f, 0.01f, 0.0f));

    // Planet atmosphere
    {
      m_atmosphere = new PhysicsObject();
      m_atmosphere->SetCollisionsEnabled(false);
      m_atmosphere->SetPosition(PLANET_POSITION);

      // Spherical collision shape bigger than the planet
      ICollisionShape *shape = new SphereCollisionShape(PLANET_RADIUS * 1.5f);
      m_atmosphere->AddCollisionShape(shape);
      m_atmosphere->AutoResizeBoundingBox();

      // Collision handler
      m_atmosphere->SetOnCollisionCallback([this](PhysicsObject *a, PhysicsObject *b) {
        bool valid = !(b == m_planet->Physics() || b == m_target->Physics() || b == m_lampPost->Physics());

        if (valid)
        {
          // Dampen velocity
          b->SetLinearVelocity(b->GetLinearVelocity() * 0.95f);
          b->SetAngularVelocity(b->GetAngularVelocity() * 0.95f);

          // Reduce opacity
          Object *obj = b->GetAssociatedObject();
          if (obj != nullptr)
          {
            Vector4 col = obj->GetColour();
            col.w *= (0.995f - (b->GetLinearVelocity().LengthSquared() * 1e-4f));
            obj->SetColour(col);
          }
        }

        return false;
      });

      PhysicsEngine::Instance()->AddPhysicsObject(m_atmosphere);
    }
  }

  // Create target
  {
    m_target = new ObjectMesh("target");

    m_target->SetMesh(m_targetMesh, false);

    m_target->CreatePhysicsNode();
    m_target->Physics()->SetPosition(Vector3(PLANET_RADIUS + 1.0f, 0.0f, 0.0f));
    m_target->Physics()->SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0.0f, 0.0f, 1.0f), 90.0f));

    m_target->Physics()->SetInverseMass(0.0f);

    HullCollisionShape *shape = new HullCollisionShape();
    shape->BuildFromMesh(m_targetMesh);
    m_target->Physics()->AddCollisionShape(shape);
    m_target->Physics()->SetInverseInertia(shape->BuildInverseInertia(0.0f));
    m_target->Physics()->SetFriction(1.0f);
    m_target->Physics()->SetElasticity(0.0f);

    m_target->Physics()->AutoResizeBoundingBox();

    AddGameObject(m_target);

    m_target->Physics()->SetOnCollisionCallback([this](PhysicsObject *a, PhysicsObject *b) {
      // No collisions with planet
      return (b != m_planet->Physics());
    });

    // Fix target position to planet
    PhysicsEngine::Instance()->AddConstraint(new WeldConstraint(m_planet->Physics(), m_target->Physics()));
  }

  // Create lamp post
  {
    m_lampPost = new ObjectMesh("lamp_post");

    m_lampPost->SetMesh(m_lampPostMesh, false);
    m_lampPost->SetTexture(CommonMeshes::CheckerboardTex(), false);

    m_lampPost->CreatePhysicsNode();
    m_lampPost->Physics()->SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0.0f, 0.0f, -1.0f), 90.0f) *
                                          Quaternion::AxisAngleToQuaterion(Vector3(1.0f, 0.0f, 0.0f), 30.0f));
    Vector3 position(0.0f, PLANET_RADIUS, 0.0f);
    Quaternion::RotatePointByQuaternion(m_lampPost->Physics()->GetOrientation(), position);
    m_lampPost->Physics()->SetPosition(position);

    m_target->Physics()->SetInverseMass(0.0f);

    // Base shape
    {
      ICollisionShape *shape = new CuboidCollisionShape(Vector3(0.4f, 0.05f, 0.35f));
      shape->SetLocalTransform(Matrix4::Translation(Vector3(-0.12f, 0.0f, 0.0f)));
      m_lampPost->Physics()->AddCollisionShape(shape);
    }

    // Pole shape
    {
      ICollisionShape *shape = new CuboidCollisionShape(Vector3(0.08f, 1.8f, 0.08f));
      shape->SetLocalTransform(Matrix4::Translation(Vector3(-0.36f, 1.8f, 0.0f)));
      m_lampPost->Physics()->AddCollisionShape(shape);
    }

    // Top bar shape
    {
      ICollisionShape *shape = new CuboidCollisionShape(Vector3(0.3f, 0.08f, 0.08f));
      shape->SetLocalTransform(Matrix4::Translation(Vector3(-0.1f, 3.6f, 0.0f)));
      m_lampPost->Physics()->AddCollisionShape(shape);
    }

    // Top lamp shape
    {
      ICollisionShape *shape = new CuboidCollisionShape(Vector3(0.38f, 0.08f, 0.38f));
      shape->SetLocalTransform(Matrix4::Translation(Vector3(0.18f, 3.4f, 0.0f)));
      m_lampPost->Physics()->AddCollisionShape(shape);
    }

    // Main lamp shape
    {
      ICollisionShape *shape = new CuboidCollisionShape(Vector3(0.22f, 0.3f, 0.22f));
      shape->SetLocalTransform(Matrix4::Translation(Vector3(0.18f, 3.2f, 0.0f)));
      m_lampPost->Physics()->AddCollisionShape(shape);
    }

    m_lampPost->Physics()->AutoResizeBoundingBox();

    AddGameObject(m_lampPost);

    m_lampPost->Physics()->SetOnCollisionCallback([this](PhysicsObject *a, PhysicsObject *b) {
      // No collisions with planet
      return (b != this->m_planet->Physics());
    });

    // Fix position to planet
    PhysicsEngine::Instance()->AddConstraint(new WeldConstraint(m_planet->Physics(), m_lampPost->Physics()));
  }

  // Objects in space
  {
    // Force at rest normally due to no gravity, add gravity on first collision
    auto gravityOnCollide = [this](PhysicsObject *a, PhysicsObject *b) {
      a->SetGravitationTarget(this->m_planet->Physics());
      return true;
    };

    // Cube
    {
      const Vector3 DIMENSIONS(1.0f, 1.2f, 0.8f);
      const float INVERSE_MASS = 0.1f;

      m_spaceCube = new ObjectMesh("space_cube");
      m_spaceCube->SetMesh(CommonMeshes::Cube(), false);
      m_spaceCube->SetLocalTransform(Matrix4::Scale(DIMENSIONS));

      m_spaceCube->CreatePhysicsNode();
      m_spaceCube->Physics()->SetPosition(Vector3(0.0f, PLANET_RADIUS + 30.0f, 0.0f));
      m_spaceCube->Physics()->SetInverseMass(INVERSE_MASS);
      m_spaceCube->Physics()->SetOnCollisionCallback(gravityOnCollide);

      ICollisionShape *shape = new CuboidCollisionShape(DIMENSIONS);
      m_spaceCube->Physics()->AddCollisionShape(shape);
      m_spaceCube->Physics()->SetInverseInertia(shape->BuildInverseInertia(INVERSE_MASS));

      m_spaceCube->Physics()->AutoResizeBoundingBox();
      AddGameObject(m_spaceCube);
    }

    // Sphere
    {
      const float RADIUS = 3.0f;
      const float INVERSE_MASS = 0.1f;

      m_spaceSphere = new ObjectMesh("space_sphere");
      m_spaceSphere->SetMesh(CommonMeshes::Sphere(), false);
      m_spaceSphere->SetLocalTransform(Matrix4::Scale(RADIUS));

      m_spaceSphere->CreatePhysicsNode();
      m_spaceSphere->Physics()->SetPosition(Vector3(0.0f, 0.0f, PLANET_RADIUS + 30.0f));
      m_spaceSphere->Physics()->SetInverseMass(INVERSE_MASS);
      m_spaceSphere->Physics()->SetOnCollisionCallback(gravityOnCollide);

      ICollisionShape *shape = new SphereCollisionShape(RADIUS);
      m_spaceSphere->Physics()->AddCollisionShape(shape);
      m_spaceSphere->Physics()->SetInverseInertia(shape->BuildInverseInertia(INVERSE_MASS));

      m_spaceSphere->Physics()->AutoResizeBoundingBox();
      AddGameObject(m_spaceSphere);
    }
  }

  // Test quads
  {// Quad 1
   {static const Vector3 DIMENSIONS(10.0f, 10.0f, 0.2f);

  m_testQuad1 = new ObjectMesh("test_plane_1");

  m_testQuad1->SetMesh(Mesh::GenerateQuadAlt(), true);
  m_testQuad1->SetTexture(CommonMeshes::CheckerboardTex(), false);

  m_testQuad1->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.6f));
  m_testQuad1->SetLocalTransform(Matrix4::Scale(DIMENSIONS));

  m_testQuad1->CreatePhysicsNode();
  m_testQuad1->Physics()->SetInverseMass(0.0f);

  m_testQuad1->Physics()->SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0.0f, 1.0f, 0.0f), 120.0f) *
                                         Quaternion::AxisAngleToQuaterion(Vector3(1.0f, 0.0f, 0.0f), 20.0f));
  Vector3 position(0.0f, 0.0f, PLANET_RADIUS + 1.0f);
  Quaternion::RotatePointByQuaternion(m_testQuad1->Physics()->GetOrientation(), position);
  m_testQuad1->Physics()->SetPosition(position);

  ICollisionShape *shape = new CuboidCollisionShape(DIMENSIONS);
  m_testQuad1->Physics()->AddCollisionShape(shape);

  m_testQuad1->Physics()->SetOnCollisionCallback([this](PhysicsObject *, PhysicsObject *o) {
    // No collide with planet
    return (o != m_planet->Physics());
  });

  m_testQuad1->Physics()->AutoResizeBoundingBox();
  AddGameObject(m_testQuad1);

  // Fix position to planet
  PhysicsEngine::Instance()->AddConstraint(new WeldConstraint(m_planet->Physics(), m_testQuad1->Physics()));
}

// Quad 2
{
  static const Vector3 DIMENSIONS(10.0f, 10.0f, 0.2f);

  m_testQuad2 = new ObjectMesh("test_plane_1");

  m_testQuad2->SetMesh(Mesh::GenerateQuadAlt(), true);
  m_testQuad2->SetTexture(CommonMeshes::CheckerboardTex(), false);

  m_testQuad2->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.6f));
  m_testQuad2->SetLocalTransform(Matrix4::Scale(DIMENSIONS));

  m_testQuad2->CreatePhysicsNode();
  m_testQuad2->Physics()->SetInverseMass(0.0f);

  m_testQuad2->Physics()->SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0.0f, 1.0f, 0.0f), 118.0f) *
                                         Quaternion::AxisAngleToQuaterion(Vector3(1.0f, 0.0f, 0.0f), 20.0f));
  Vector3 position(0.0f, 0.0f, PLANET_RADIUS + 2.5f);
  Quaternion::RotatePointByQuaternion(m_testQuad2->Physics()->GetOrientation(), position);
  m_testQuad2->Physics()->SetPosition(position);
  m_testQuad2->Physics()->SetOrientation(m_testQuad2->Physics()->GetOrientation() *
                                         Quaternion::AxisAngleToQuaterion(Vector3(0.0f, -1.0f, 0.0f), 90.0f));

  ICollisionShape *shape = new CuboidCollisionShape(DIMENSIONS);
  m_testQuad2->Physics()->AddCollisionShape(shape);

  m_testQuad2->Physics()->SetOnCollisionCallback([this](PhysicsObject *, PhysicsObject *o) {
    // No collide with planet
    return (o != m_planet->Physics());
  });

  m_testQuad2->Physics()->AutoResizeBoundingBox();
  AddGameObject(m_testQuad2);

  // Fix position to planet
  PhysicsEngine::Instance()->AddConstraint(new WeldConstraint(m_planet->Physics(), m_testQuad2->Physics()));
}
}

// Soft body
{
  m_softBody = CommonUtils::BuildSoftBodyDemo(Vector3(0.0f, PLANET_RADIUS, 0.0f), 10, 10, 2.0f, 2.0f, m_planet->Physics());

  // Fix position to planet
  PhysicsEngine::Instance()->AddConstraint(new WeldConstraint(m_planet->Physics(), m_softBody->Physics()));

  AddGameObject(m_softBody);
}

// Create player
m_player = new Player(this, m_broker);

// Check for server connection (should have probably happened in the time taken for world generation, etc. (on a local
// connection at least))
if (m_broker->IsConnectedToServer())
  NCLDebug::Log("Connected to server!");
else
  NCLERROR("Cannot connect to server.");

// Publish collision detection messages at 15Hz
m_physicsNetClient->StartUpdateThread(0.016f);
}

void CourseworkScene::OnCleanupScene()
{
  // Remove player
  delete m_player;
  m_player = nullptr;

  // Close networking
  PubSubBrokerNetNode *broker = m_broker;
  {
    std::lock_guard<std::mutex> lock(m_broker->Mutex());

    // Remove announce client
    m_broker->UnSubscribeFromAll(m_netAnnounceClient);
    delete m_netAnnounceClient;
    m_netAnnounceClient = nullptr;

    // Remove physics control client
    m_broker->UnSubscribeFromAll(m_physicsNetClient);
    delete m_physicsNetClient;
    m_physicsNetClient = nullptr;

    // Close broker network node
    m_broker->Release();
    m_broker = nullptr;
  }

  // Stop network update thread
  if (m_networkThread.joinable())
    m_networkThread.join();

  delete broker;

  NCLDebug::Log("Disconnected from server.");

  // Reset state machines
  m_debugDrawStateMachine.Reset();
  m_broadphaseModeStateMachine.Reset();
  m_integrationModeStateMachine.Reset();

  // Cleanup object pointers
  m_planet = nullptr;
  m_atmosphere = nullptr;
  m_target = nullptr;
  m_lampPost = nullptr;
  m_spaceCube = nullptr;
  m_spaceSphere = nullptr;
  m_testQuad1 = nullptr;
  m_testQuad2 = nullptr;
  m_softBody = nullptr;

  Scene::OnCleanupScene();
}

void CourseworkScene::OnUpdateScene(float dt)
{
  // Update state machines
  m_debugDrawStateMachine.Update(dt);
  m_broadphaseModeStateMachine.Update(dt);
  m_integrationModeStateMachine.Update(dt);
  m_player->Update(dt);

  // Atmosphere status and toggle
  NCLDebug::AddStatusEntry(Vector4(0.5f, 1.0f, 0.0f, 1.0f), "Atmosphere: %s", (m_atmosphere->CanCollide() ? "on" : "off"));
  if (Window::GetKeyboard()->KeyTriggered(ATMOSPHERE_TOGGLE_KEY))
    m_atmosphere->SetCollisionsEnabled(!m_atmosphere->CanCollide());

  // Show player score
  if (m_player != nullptr)
  {
    NCLDebug::AddStatusEntry(Vector4(), "");
    NCLDebug::AddStatusEntry(Vector4(0.0f, 1.0f, 0.2f, 1.0f), "Score: %5.0f", m_player->GetScoreCounter().GetScore());
    NCLDebug::AddStatusEntry(Vector4(0.0f, 1.0f, 0.2f, 1.0f), "Shots Remaining: %d", m_player->NumShootablesRemaining());
  }
}

/**
 * @brief Runs the update loop required for the broker network.
 */
void CourseworkScene::BrokerNetworkLoop()
{
  while (m_broker != nullptr)
  {
    {
      std::lock_guard<std::mutex> lock(m_broker->Mutex());

      // Do another text just in case broker was deleted while waiting for mutex lock
      if (m_broker != nullptr)
      {
        float dt = m_networkTimer.GetTimedMS() * 0.001f;
        m_broker->PumpNetwork(dt);
      }
    }

    Sleep(0);
  }
}
