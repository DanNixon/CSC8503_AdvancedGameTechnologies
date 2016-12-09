#pragma once

#include <nclgl\Mesh.h>
#include <ncltech\NetSyncStateMachine.h>
#include <ncltech\Scene.h>
#include <queue>

class CourseworkScene : public Scene
{
public:
  CourseworkScene(const std::string &friendly_name);
  virtual ~CourseworkScene();

  virtual void OnInitializeScene() override;
  virtual void OnCleanupScene() override;
  virtual void OnUpdateScene(float dt) override;

protected:
  StateMachine m_debugDrawStateMachine;     //!< State machine controlling debug draw
  NetSyncStateMachine m_playerStateMachine; //!< State machine controlling player behaviour

  Object *m_planet;                   //!< Planet object
  std::queue<Object *> m_shotSpheres; //!< Queue of spheres shot by the player
};