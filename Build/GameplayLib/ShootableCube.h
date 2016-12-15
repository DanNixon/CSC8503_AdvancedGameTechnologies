#pragma once

#include "IShootable.h"

/**
 * @class ShootableCube
 * @author Dan Nixon
 * @brief Cube that can be shot by the player.
 */
class ShootableCube : public IShootable
{
public:
  ShootableCube(Player *owner, float lifetime = 10.0f, const Vector3 &halfDims = Vector3(0.5f, 0.5f, 0.5f), float inverseMass = 1.0f);
  virtual ~ShootableCube();
};
