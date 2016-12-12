#pragma once

#include <ncltech/ObjectMesh.h>
#include <ncltech/StateMachine.h>

/**
 * @class Ball
 * @author Dan Nixon
 * @brief Balls that can be shot from the perspective of the player.
 */
class Ball : public ObjectMesh
{
public:
  Ball(float radius = 0.5f, float inverseMass = 1.0f, float lifetime = 10.0f);
  virtual ~Ball();

  /**
   * @brief Gets the state machine for this ball.
   * @return Reference to state machine
   */
  inline StateMachine &GetStateMachine()
  {
    return m_stateMachine;
  }

  inline bool IsDead() const
  {
    return m_dead;
  }

protected:
  float m_lifetime;            //!< Lifetime in seconds
  StateMachine m_stateMachine; //!< AI for the ball

  bool m_dead;
  bool m_hitTarget;
};