#pragma once

#include "IShootable.h"

class ShootableBall : public IShootable
{
public:
  ShootableBall(float radius = 0.5f, float inverseMass = 1.0f, float lifetime = 10.0f);
  virtual ~ShootableBall();
};