#include "Ball.h"

Ball::Ball()
{
  // State machine
  {
    // Idle state
    State *idle = new State("idle", m_stateMachine.RootState(), &m_stateMachine);

    // Shot state
    State *shot = new State("shot", m_stateMachine.RootState(), &m_stateMachine);

    // Target hit state
    State *targetHit = new State("target_hit", shot, &m_stateMachine);

    // Die state
    State *die = new State("die", shot, &m_stateMachine);

    // Set default state
    m_stateMachine.SetDefaultState(idle);
  }
}

Ball::~Ball()
{
}