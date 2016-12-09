#pragma once

#include <ncltech\CommonUtils.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\Scene.h>
#include <ncltech\SceneManager.h>
#include <ncltech\SortAndSweepBroadphase.h>

class Test3_Target : public Scene
{
public:
  Test3_Target(const std::string &friendly_name)
      : Scene(friendly_name)
  {
  }

  virtual void OnInitializeScene() override
  {
    SceneManager::Instance()->GetCamera()->SetPosition(Vector3(0.0f, 25.0f, 25.0f));

    PhysicsEngine::Instance()->SetBroadphase(new SortAndSweepBroadphase());
    // PhysicsEngine::Instance()->SetDebugDrawFlags(DEBUGDRAW_FLAGS_CONSTRAINT);

    // TODO
  }
};