#pragma once

#include <ncltech/ObjectMesh.h>
#include <ncltech/StateMachine.h>

class Player;

/**
 * @class Ball
 * @author Dan Nixon
 * @brief Balls that can be shot from the perspective of the player.
 */
class IShootable : public ObjectMesh
{
public:
  IShootable(Player *owner, float lifetime = 10.0f);
  virtual ~IShootable();

  /**
   * @brief Gets a pointer to the player that shot this thing.
   * @return Pointer to owning player
   */
  Player *GetOwner()
  {
    return m_owner;
  }

  /**
   * @brief Gets the state machine for this ball.
   * @return Reference to state machine
   */
  inline StateMachine &GetStateMachine()
  {
    return m_stateMachine;
  }

  /**
   * @brief Gets the value of the flag indicating this ball should be removed.
   * @return Expired flag
   */
  inline bool HasExpired() const
  {
    return m_expired;
  }

protected:
  void PostCreate(float inverseMass);

protected:
  Player *m_owner; //!< Player that shot this thing

  float m_lifetime;            //!< Lifetime in seconds
  StateMachine m_stateMachine; //!< AI for the ball

  bool m_hitTarget; //!< Flag indicating that this ball has collided with the target
  bool m_expired;   //!< Flag indicating that this ball should be removed
};