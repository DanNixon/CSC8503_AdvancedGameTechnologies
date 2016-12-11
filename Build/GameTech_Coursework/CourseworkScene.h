#pragma once

#include <nclgl\Mesh.h>
#include <ncltech\NetSyncStateMachine.h>
#include <ncltech\Scene.h>
#include <ncltech\ObjectMesh.h>
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

  GLuint m_planetTex; //!< Texture of planet surface
  Mesh *m_targetMesh; //!< Graphical mesh for the target

  ObjectMesh *m_planet;               //!< Planet object
  ObjectMesh *m_target;               //!< Target object
  std::queue<Object *> m_shotSpheres; //!< Queue of spheres shot by the player
};