#pragma once

#include <ncltech\Scene.h>
#include <ncltech\StateMachine.h>

#include "IShootable.h"
#include "LocalScore.h"

/**
 * @class Player
 * @author Dan Nixon
 * @brief Represents a player in the simulation.
 */
class Player
{
public:
  /**
   * @brief Key used to shoot a cube from the camera view point.
   */
  static const KeyboardKeys SHOOT_CUBE_KEY = KeyboardKeys::KEYBOARD_H;

  /**
   * @brief Key used to shoot a sphere from the camera view point.
   */
  static const KeyboardKeys SHOOT_BALL_KEY = KeyboardKeys::KEYBOARD_J;

  /**
   * @brief Key used to reset the player state.
   */
  static const KeyboardKeys PLAYER_RESET_KEY = KeyboardKeys::KEYBOARD_K;

public:
  Player(Scene *scene = nullptr, PubSubBroker * broker = nullptr);
  virtual ~Player();

  /**
   * @brief Gets the local score counter for the player.
   * @return Reference to score counter
   */
  LocalScore &GetScoreCounter()
  {
    return m_score;
  }

  /**
   * @brief Gets the remaining number of shots the player can take.
   * @return Shots remaining
   */
  int NumShootablesRemaining() const
  {
    return m_numShootablesRemaining;
  }

  void Reset();
  void Update(float dt);

protected:
  void ShootFromCamera(IShootable *shootable, float power = 1.0f);

protected:
  Scene *m_scene;                         //!< Scene this player operates in
  StateMachine m_playerStateMachine;      //!< State machine controlling player behaviour
  LocalScore m_score;                     //!< Player's score
  int m_numShootablesRemaining;           //!< Number of things the player has left to shoot
  std::vector<IShootable *> m_shotThings; //!< Things shot by the player
};
