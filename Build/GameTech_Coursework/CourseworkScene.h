#pragma once

#include <nclgl\Mesh.h>
#include <ncltech\NetSyncStateMachine.h>
#include <ncltech\ObjectMesh.h>
#include <ncltech\Scene.h>
#include <vector>

#include <GameplayLib\Player.h>

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

  GLuint m_planetTex;   //!< Texture of planet surface
  Mesh *m_targetMesh;   //!< Graphical mesh for the target
  Mesh *m_lampPostMesh; //!< Graphical mesh for the lamp post

  ObjectMesh *m_planet;   //!< Planet object
  ObjectMesh *m_target;   //!< Target object
  ObjectMesh *m_lampPost; //!< Lamp post object

  Player *m_player; //!< Player
};
