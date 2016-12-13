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

/**
 * @brief Radius of the planet.
 */
const float CourseworkScene::PLANET_RADIUS = 100.0f;

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
}

/**
 * @brief Creates a new courseowrk scene.
 */
CourseworkScene::CourseworkScene()
    : Scene("GameTech coursework")
    , m_planetTex(0)
    , m_targetMesh(nullptr)
    , m_lampPostMesh(nullptr)
    , m_planet(nullptr)
    , m_target(nullptr)
    , m_lampPost(nullptr)
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

    // State transitions
    sortAndSweepX->AddTransferFromTest(
        [sortAndSweepY]() { return Window::GetKeyboard()->KeyTriggered(BROADPHASE_MODE_KEY) ? sortAndSweepY : nullptr; });

    sortAndSweepY->AddTransferFromTest(
        [sortAndSweepZ]() { return Window::GetKeyboard()->KeyTriggered(BROADPHASE_MODE_KEY) ? sortAndSweepZ : nullptr; });

    sortAndSweepZ->AddTransferFromTest(
        [bruteForce]() { return Window::GetKeyboard()->KeyTriggered(BROADPHASE_MODE_KEY) ? bruteForce : nullptr; });

    bruteForce->AddTransferFromTest(
        [octree]() { return Window::GetKeyboard()->KeyTriggered(BROADPHASE_MODE_KEY) ? octree : nullptr; });

    octree->AddTransferFromTest(
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
}

void CourseworkScene::OnInitializeScene()
{
  PrintKeyMapping();

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

  // Create player
  m_player = new Player(this);

  // Create planet
  {
    m_planet = new ObjectMesh("planet");

    m_planet->SetMesh(CommonMeshes::Sphere(), false);
    m_planet->SetTexture(m_planetTex, false);
    m_planet->SetColour(Vector4(0.5f, 0.5f, 0.5f, 1.0f));

    m_planet->SetLocalTransform(Matrix4::Scale(Vector3(PLANET_RADIUS, PLANET_RADIUS, PLANET_RADIUS)));

    m_planet->CreatePhysicsNode();
    m_planet->Physics()->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
    m_planet->Physics()->SetInverseMass(0.0f);

    ICollisionShape *shape = new SphereCollisionShape(PLANET_RADIUS);
    m_planet->Physics()->AddCollisionShape(shape);
    m_planet->Physics()->SetInverseInertia(shape->BuildInverseInertia(0.0f));

    // Never perform at rest test on planet (should always rotate)
    m_planet->Physics()->SetRestVelocityThreshold(0.0f);

    m_planet->Physics()->AutoResizeBoundingBox();

    AddGameObject(m_planet);
  }

  // Create target
  {
    m_target = new ObjectMesh("target");

    m_target->SetMesh(m_targetMesh, false);

    m_target->CreatePhysicsNode();
    m_target->Physics()->SetPosition(Vector3(PLANET_RADIUS + 5.0f, 0.0f, 0.0f));
    m_target->Physics()->SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0.0f, 0.0f, 1.0f), 90.0f));

    m_target->Physics()->SetInverseMass(0.0f);

    HullCollisionShape *shape = new HullCollisionShape();
    shape->BuildFromMesh(m_targetMesh);
    m_target->Physics()->AddCollisionShape(shape);
    m_target->Physics()->SetInverseInertia(shape->BuildInverseInertia(0.0f));

    m_target->Physics()->AutoResizeBoundingBox();

    AddGameObject(m_target);

    m_target->Physics()->SetOnCollisionCallback([this](PhysicsObject *a, PhysicsObject *b) {
      // No collisions with planet
      return (b != this->m_planet->Physics());
    });

    // Fix target position to planet
    PhysicsEngine::Instance()->AddConstraint(new WeldConstraint(m_planet->Physics(), m_target->Physics()));
  }

  // Create lamp post
  {
    m_lampPost = new ObjectMesh("lamp_post");

    m_lampPost->SetMesh(m_lampPostMesh, false);
    m_lampPost->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    m_lampPost->CreatePhysicsNode();
    m_lampPost->Physics()->SetPosition(Vector3(PLANET_RADIUS + 10.0f, 0.0f, 0.0f));
    // m_lampPost->Physics()->SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0.0f, 0.0f, 1.0f), 90.0f));

    m_target->Physics()->SetInverseMass(0.0f);

    ICollisionShape *shape1 = new CuboidCollisionShape(Vector3(1.0f, 1.0f, 1.0f));
    m_lampPost->Physics()->AddCollisionShape(shape1);

    m_lampPost->Physics()->AutoResizeBoundingBox();

    AddGameObject(m_lampPost);

    m_lampPost->Physics()->SetOnCollisionCallback([this](PhysicsObject *a, PhysicsObject *b) {
      // No collisions with planet
      return (b != this->m_planet->Physics());
    });

    // Fix target position to planet
    PhysicsEngine::Instance()->AddConstraint(new WeldConstraint(m_planet->Physics(), m_lampPost->Physics()));
  }
}

void CourseworkScene::OnCleanupScene()
{
  // Reset state machines
  m_debugDrawStateMachine.Reset();
  m_broadphaseModeStateMachine.Reset();
  m_integrationModeStateMachine.Reset();

  // Remove player
  delete m_player;
  m_player = nullptr;

  // Cleanup object pointers
  m_planet = nullptr;
  m_target = nullptr;
  m_lampPost = nullptr;

  Scene::OnCleanupScene();
}

void CourseworkScene::OnUpdateScene(float dt)
{
  // Update state machines
  m_debugDrawStateMachine.Update(dt);
  m_broadphaseModeStateMachine.Update(dt);
  m_integrationModeStateMachine.Update(dt);
  m_player->Update(dt);

  // Show player score
  if (m_player != nullptr)
  {
    NCLDebug::AddStatusEntry(Vector4(), "");
    NCLDebug::AddStatusEntry(Vector4(0.0f, 1.0f, 0.2f, 1.0f), "Score: %5.0f", m_player->GetScoreCounter().GetScoreCounter());
  }

  // Add planet rotation
  m_planet->Physics()->SetAngularVelocity(Vector3(0.0f, 0.01f, 0.0f));
}
