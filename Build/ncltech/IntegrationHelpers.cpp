#include "IntegrationHelpers.h"

/**
 * @brief Performs RK2 integration for linear motion.
 * @param state Reference to motion state (containing state at start of frame)
 * @param dt Update timestep
 */
void IntegrationHelpers::RK2(State &state, float dt)
{
  State a, b, c, d;

  a = Evaluate(state, 0.0f, State());
  b = Evaluate(state, dt * 0.5f, a);

  Vector3 dxdt = (a.velocity + b.velocity) * 0.5f;
  Vector3 dvdt = (a.acceleration + b.acceleration) * 0.5f;

  state.position += dxdt * dt;
  state.velocity += dvdt * dt;
}

/**
 * @brief Performs RK4 integration for linear motion.
 * @param state Reference to motion state (containing state at start of frame)
 * @param dt Update timestep
 */
void IntegrationHelpers::RK4(State &state, float dt)
{
  State a, b, c, d;

  a = Evaluate(state, 0.0f, State());
  b = Evaluate(state, dt * 0.5f, a);
  c = Evaluate(state, dt * 0.5f, b);
  d = Evaluate(state, dt, c);

  Vector3 dxdt = (a.velocity + (b.velocity + c.velocity) * 2.0f + d.velocity) * 1.0f / 6.0f;
  Vector3 dvdt = (a.acceleration + (b.acceleration + c.acceleration) * 2.0f + d.acceleration) * 1.0f / 6.0f;

  state.position += dxdt * dt;
  state.velocity += dvdt * dt;
}

/**
 * @brief Performs an integration evaluation.
 * @param initial Initial state
 * @param dt Update timestep
 * @param derivative Derivative state
 */
IntegrationHelpers::State IntegrationHelpers::Evaluate(State initial, float dt, const State &derivative)
{
  initial.position += derivative.velocity * dt;
  initial.velocity += derivative.acceleration * dt;

  State out;
  out.velocity = initial.velocity;
  out.acceleration = initial.acceleration;

  return out;
}
