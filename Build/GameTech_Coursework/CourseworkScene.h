#pragma once

#include <nclgl\Mesh.h>
#include <ncltech\NetSyncStateMachine.h>
#include <ncltech\ObjectMesh.h>
#include <ncltech\Scene.h>
#include <vector>

#include "Ball.h"

/**
 * @class CourseworkScene
 * @author Dan Nixon
 * @brief Courseowrk demo scene.
 */
class CourseworkScene : public Scene
{
public:
  static const float PLANET_RADIUS;

  /**
   * @brief Key used to toggle broadphase collision detection mode.
   */
  static const KeyboardKeys BROADPHASE_MODE_KEY = KeyboardKeys::KEYBOARD_B;

  /**
   * @brief Key used to toggle integration mode.
   */
  static const KeyboardKeys INTEGRATION_MODE_KEY = KeyboardKeys::KEYBOARD_I;

  /**
   * @brief Key used to toggle physics debug draw mode.
   */
  static const KeyboardKeys PHYSICS_DEBUG_VIEW_KEY = KeyboardKeys::KEYBOARD_M;

  /**
   * @brief Key used to shoot a sphere from the camera view point.
   */
  static const KeyboardKeys SHOOT_BALL_KEY = KeyboardKeys::KEYBOARD_J;

  /**
   * @brief Key used to reset the player state.
   */
  static const KeyboardKeys PLAYER_RESET_KEY = KeyboardKeys::KEYBOARD_K;

public:
  static void PrintKeyMapping();

public:
  CourseworkScene();
  virtual ~CourseworkScene();

  virtual void OnInitializeScene() override;
  virtual void OnCleanupScene() override;
  virtual void OnUpdateScene(float dt) override;

protected:
  StateMachine m_debugDrawStateMachine;       //!< State machine controlling debug draw
  StateMachine m_broadphaseModeStateMachine;  //!< State machine controlling broadphase mode
  StateMachine m_integrationModeStateMachine; //!< State machine controlling broadphase mode
  StateMachine m_playerStateMachine;          //!< State machine controlling player behaviour

  GLuint m_planetTex;   //!< Texture of planet surface
  Mesh *m_targetMesh;   //!< Graphical mesh for the target
  Mesh *m_lampPostMesh; //!< Graphical mesh for the lamp post

  ObjectMesh *m_planet;              //!< Planet object
  ObjectMesh *m_target;              //!< Target object
  std::vector<Ball *> m_shotSpheres; //!< Balls shot by the player
};
