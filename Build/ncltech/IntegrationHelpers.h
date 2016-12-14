#pragma once

#include <nclgl\Vector3.h>

/**
 * @class IntegrationHelpers
 * @author Dan Nixon
 * @brief COntains helper functions for performing "midpoint" integration.
 */
class IntegrationHelpers
{
public:
  /**
   * @brief Container for a single set of linear motion state information.
   */
  struct State
  {
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
  };

public:
  static void RK2(State & state, float dt);
  static void RK4(State & state, float dt);

  static State Evaluate(State initial, float dt, const State & derivative);
};