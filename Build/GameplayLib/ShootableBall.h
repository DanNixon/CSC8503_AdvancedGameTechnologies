#pragma once

#include "IShootable.h"

/**
 * @class ShootableBall
 * @author Dan Nixon
 * @brief Ball that can be shot by the player.
 */
class ShootableBall : public IShootable
{
public:
  ShootableBall(Player *owner, float radius = 0.5f, float inverseMass = 1.0f, float lifetime = 10.0f);
  virtual ~ShootableBall();
};
