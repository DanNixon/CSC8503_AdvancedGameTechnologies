#include "TestScene.h"

#include <nclgl\Vector4.h>
#include <ncltech\CommonUtils.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\SceneManager.h>
#include <ncltech\OctreeBroadphase.h>
#include <ncltech\BruteForceBroadphase.h>

using namespace CommonUtils;

TestScene::TestScene(const std::string &friendly_name)
    : Scene(friendly_name)
    , m_AccumTime(0.0f)
    , m_pPlayer(NULL)
{
}

TestScene::~TestScene()
{
}

void TestScene::OnInitializeScene()
{
  PhysicsEngine::Instance()->SetPaused(true);

  PhysicsEngine::Instance()->SetBroadphase(new OctreeBroadphase(100, 2, new BruteForceBroadphase()));

  // PhysicsEngine::Instance()->SetDebugDrawFlags(DEBUGDRAW_FLAGS_BROADPHASE);
  PhysicsEngine::Instance()->SetDebugDrawFlags(DEBUGDRAW_FLAGS_BROADPHASE | DEBUGDRAW_FLAGS_BROADPHASE_PAIRS);

  // Set the camera position
  SceneManager::Instance()->GetCamera()->SetPosition(Vector3(15.0f, 10.0f, -15.0f));
  SceneManager::Instance()->GetCamera()->SetYaw(140.f);
  SceneManager::Instance()->GetCamera()->SetPitch(-20.f);

  m_AccumTime = 0.0f;

  // Example usage of Log and LogE inside NCLDebug functionality
  NCLDebug::Log("This is a log entry");
  NCLERROR("THIS IS AN ERROR!");

  //<--- SCENE CREATION --->
  // Create Ground
  AddGameObject(BuildCuboidObject("ground", Vector3(0.0f, -1.0f, 0.0f), Vector3(20.0f, 1.0f, 20.0f), true, 0.0f, true,
                                        false, Vector4(0.2f, 0.5f, 1.0f, 1.0f)));

  // Create Player (See OnUpdateScene)
  m_pPlayer = BuildCuboidObject("Player1",                        
                                Vector3(5.f, 0.5f, 0.0f),         
                                Vector3(0.5f, 0.5f, 1.0f),        
                                true,                             
                                0.0f,                              
                                true,                             
                                false,                            
                                Vector4(0.1f, 0.1f, 0.1f, 1.0f));
  AddGameObject(m_pPlayer);

  auto create_cube_tower = [&](const Vector3 &offset, float cubewidth) {
    const Vector3 halfdims = Vector3(cubewidth, cubewidth, cubewidth) * 0.5f;
    for (int x = 0; x < 2; ++x)
    {
      for (int y = 0; y < 6; ++y)
      {
        uint idx = x * 5 + y;
        Vector4 colour = GenColour(idx / 10.f, 0.5f);
        Vector3 pos = offset + Vector3(x * cubewidth, 1e-3f + y * cubewidth, cubewidth * (idx % 2 == 0) ? 0.5f : -0.5f);

        Object *cube = BuildCuboidObject("cube",
                                         pos,      
                                         halfdims, 
                                         true,     
                                         1.0f,     
                                         true,     
                                         true,     
                                         colour);  
        AddGameObject(cube);
      }
    }
  };

  auto create_ball_cube = [&](const Vector3 &offset, const Vector3 &scale, float ballsize) {
    const int dims = 5;
    const Vector4 col = Vector4(1.0f, 0.5f, 0.2f, 1.0f);

    for (int x = 0; x < dims; ++x)
    {
      for (int y = 0; y < dims; ++y)
      {
        for (int z = 0; z < dims; ++z)
        {
          Vector3 pos = offset + Vector3(scale.x * x, scale.y * y, scale.z * z);
          Object *sphere = BuildSphereObject("sphere",
                                             pos,      
                                             ballsize, 
                                             true,     
                                             1.0f,     
                                             true,     
                                             true,     
                                             col);  
          AddGameObject(sphere);
        }
      }
    }
  };

  // Create Cube Towers
  //create_cube_tower(Vector3(3.0f, 0.5f, 3.0f), 1.0f);
  //create_cube_tower(Vector3(-3.0f, 0.5f, -3.0f), 1.0f);

  // Create Test Ball Pit
  create_ball_cube(Vector3(-8.0f, 0.5f, 12.0f), Vector3(0.5f, 0.5f, 0.5f), 0.1f);
  //create_ball_cube(Vector3(8.0f, 0.5f, 12.0f), Vector3(0.3f, 0.3f, 0.3f), 0.1f);
  //create_ball_cube(Vector3(-8.0f, 0.5f, -12.0f), Vector3(0.2f, 0.2f, 0.2f), 0.1f);
  //create_ball_cube(Vector3(8.0f, 0.5f, -12.0f), Vector3(0.5f, 0.5f, 0.5f), 0.1f);
}

void TestScene::OnCleanupScene()
{
  // Just delete all created game objects
  //  - this is the default command on any Scene instance so we don't really
  //  need to override this function here.
  Scene::OnCleanupScene();
}

void TestScene::OnUpdateScene(float dt)
{
  m_AccumTime += dt;

  // You can add status entries to the top left from anywhere in the program
  NCLDebug::AddStatusEntry(Vector4(1.0f, 0.2f, 0.2f, 1.0f), "Welcome to the Game Tech Framework!");
  NCLDebug::AddStatusEntry(Vector4(1.0f, 0.4f, 0.4f, 1.0f), "   \x01 You can move the black car with the arrow keys");

  // You can print text using 'printf' formatting
  bool donkeys = false;
  NCLDebug::AddStatusEntry(Vector4(1.0f, 0.4f, 0.4f, 1.0f), "   \x01 The %s in this scene are dragable",
                           donkeys ? "donkeys" : "cubes");

  // Lets sun a little bit...
  Vector3 invLightDir =
      Matrix4::Rotation(15.0f * dt, Vector3(0.0f, 1.0f, 0.0f)) * SceneManager::Instance()->GetInverseLightDirection();
  SceneManager::Instance()->SetInverseLightDirection(invLightDir);

  // Or move our car around the scene..
  {
    const float mv_speed = 10.f * dt;  // Motion: Meters per second
    const float rot_speed = 90.f * dt; // Rotation: Degrees per second

    bool boosted = false;

    PhysicsObject *pobj = m_pPlayer->Physics();
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP))
    {
      pobj->SetPosition(pobj->GetPosition() + pobj->GetOrientation().ToMatrix3() * Vector3(0.0f, 0.0f, -mv_speed));
      boosted = true;
    }

    if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN))
    {
      pobj->SetPosition(pobj->GetPosition() + pobj->GetOrientation().ToMatrix3() * Vector3(0.0f, 0.0f, mv_speed / 2.f));
    }

    if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT))
    {
      pobj->SetOrientation(pobj->GetOrientation() * Quaternion::AxisAngleToQuaterion(Vector3(0.0f, 1.0f, 0.0f), rot_speed));
    }

    if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT))
    {
      pobj->SetOrientation(pobj->GetOrientation() * Quaternion::AxisAngleToQuaterion(Vector3(0.0f, 1.0f, 0.0f), -rot_speed));
    }

    // Also (and importantly), as the projMatrix/viewMatrix is all abstracted
    // away
    //  we can now use debug draw tools to render geometry in world-space from
    //  anywhere
    //  in the program. Very useful for debugging!
    if (boosted)
    {
      // Draw the rocket booster on the car using NCLDebug
      Vector3 backward_dir = pobj->GetOrientation().ToMatrix3() * Vector3(0, 0, 1);
      NCLDebug::DrawPoint(pobj->GetPosition() + backward_dir, 0.3f, Vector4(1.f, 0.7f, 0.0f, 1.0f));
      NCLDebug::DrawPoint(pobj->GetPosition() + backward_dir * 1.333f, 0.26f, Vector4(0.9f, 0.5f, 0.0f, 1.0f));
      NCLDebug::DrawPoint(pobj->GetPosition() + backward_dir * 1.666f, 0.23f, Vector4(0.8f, 0.3f, 0.0f, 1.0f));
      NCLDebug::DrawPoint(pobj->GetPosition() + backward_dir * 2.f, 0.2f, Vector4(0.7f, 0.2f, 0.0f, 1.0f));
    }
  }
}