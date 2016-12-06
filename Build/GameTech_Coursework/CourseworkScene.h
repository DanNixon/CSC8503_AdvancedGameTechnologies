#pragma once

#include <nclgl\Mesh.h>
#include <ncltech\Scene.h>
#include <ncltech\StateMachine.h>

class CourseworkScene : public Scene
{
public:
  CourseworkScene(const std::string &friendly_name);
  virtual ~CourseworkScene();

  virtual void OnInitializeScene() override;
  virtual void OnCleanupScene() override;
  virtual void OnUpdateScene(float dt) override;

protected:
  StateMachine m_playerStateMachine;

  Object *m_planet;
};