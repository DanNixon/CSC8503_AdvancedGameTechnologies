#pragma once

#include <ncltech\BruteForceBroadphase.h>
#include <ncltech\CommonUtils.h>
#include <ncltech\NCLDebug.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\Scene.h>
#include <ncltech\SceneManager.h>

class Test1_PointGravitation : public Scene
{
public:
  Test1_PointGravitation(const std::string &friendly_name)
      : Scene(friendly_name)
  {
  }

  virtual void OnInitializeScene() override
  {
    //SceneManager::Instance()->GetCamera()->SetPosition(Vector3(-3.0f, 10.0f, 10.0f));
    //SceneManager::Instance()->GetCamera()->SetPitch(-20.f);

    PhysicsEngine::Instance()->SetBroadphase(new BruteForceBroadphase());
    PhysicsEngine::Instance()->SetDebugDrawFlags(DEBUGDRAW_FLAGS_CONSTRAINT);
    PhysicsEngine::Instance()->SetGravity(Vector3(0.0f, 0.0f, 0.0f));

    {
      Object *handle = CommonUtils::BuildSphereObject("", Vector3(-7.f, 7.f, -5.0f),        // Position
                                                      0.5f,                                 // Radius
                                                      true,                                 // Has Physics Object
                                                      1.0f / 1e12f,                         // Inverse Mass = 1 / 1kg mass
                                                      true,                                 // Collisions enabled
                                                      true,                                 // Dragable by the user
                                                      CommonUtils::GenColour(0.45f, 0.5f)); // Color

      m_ball1 =
          CommonUtils::BuildSphereObject("", Vector3(-4.f, 7.f, -5.0f), // Position
                                         0.5f,                          // Radius
                                         true,                          // Has Physics Object
                                         1.0f / 1e12f, // 10.0f,                                // Inverse Mass = 1 / 1kg mass
                                         true,         // Collisions enabled
                                         true,         // Dragable by the user
                                         CommonUtils::GenColour(0.5f, 1.0f)); // Color

      this->AddGameObject(handle);
      this->AddGameObject(m_ball1);

      m_ball1->Physics()->SetGravitationTarget(handle->Physics());
    }

    {
      Object *handle = CommonUtils::BuildSphereObject("", Vector3(7.f, 7.f, -5.0f),         // Position
                                                      0.5f,                                 // Radius
                                                      true,                                 // Has Physics Object
                                                      0.0f,                                 // Infinate Mass
                                                      true,                                 // Collisions enabled
                                                      true,                                 // Dragable by the user
                                                      CommonUtils::GenColour(0.45f, 0.5f)); // Color

      m_ball2 =
          CommonUtils::BuildSphereObject("", Vector3(4.f, 7.f, -5.0f), // Position
                                         0.5f,                         // Radius
                                         true,                         // Has Physics Object
                                         1.0f / 1e12f, // 10.0f,                                // Inverse Mass = 1 / 1kg mass
                                         true,         // Collisions enabled
                                         true,         // Dragable by the user
                                         CommonUtils::GenColour(0.5f, 1.0f)); // Color

      this->AddGameObject(handle);
      this->AddGameObject(m_ball2);

      m_ball2->Physics()->SetGravitationTarget(handle->Physics());
    }
  }

  virtual void OnUpdateScene(float dt) override
  {
    Scene::OnUpdateScene(dt);

    // m_ball1->Physics()->ApplyForce(Vector3(0.0f, 1.0f, 0.0f));
  }

private:
  Object *m_ball1;
  Object *m_ball2;
};