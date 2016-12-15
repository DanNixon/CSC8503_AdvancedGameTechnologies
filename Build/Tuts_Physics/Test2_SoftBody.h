#pragma once

#include <ncltech\OctreeBroadphase.h>
#include <ncltech\SortAndSweepBroadphase.h>
#include <ncltech\CommonUtils.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\Scene.h>
#include <ncltech\SceneManager.h>

class Test2_SoftBody : public Scene
{
public:
  Test2_SoftBody(const std::string &friendly_name)
      : Scene(friendly_name)
  {
  }

  virtual void OnInitializeScene() override
  {
    PhysicsEngine::Instance()->SetPaused(true);

    SceneManager::Instance()->GetCamera()->SetPosition(Vector3(0.0f, 25.0f, 25.0f));

    PhysicsEngine::Instance()->SetBroadphase(new OctreeBroadphase(5, 3, new SortAndSweepBroadphase()));
    PhysicsEngine::Instance()->SetDebugDrawFlags(DEBUGDRAW_FLAGS_CONSTRAINT);

    // Build soft body
    Object * sb = CommonUtils::BuildSoftBodyDemo(20, 10, 1.5f, 2.0f);
    AddGameObject(sb);
  }
};