#pragma once

#include "IShootable.h"

class ShootableCube : public IShootable
{
public:
  ShootableCube(const Vector3 &halfDims = Vector3(0.5f, 0.5f, 0.5f), float inverseMass = 1.0f, float lifetime = 10.0f);
  virtual ~ShootableCube();
};