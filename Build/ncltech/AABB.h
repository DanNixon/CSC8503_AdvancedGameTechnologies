#pragma once

#include "Hull.h"

class AABB : public Hull
{
public:
  static const int FAR_FACE[];
  static const int NEAR_FACE[];
  static const int TOP_FACE[];
  static const int BOTTOM_FACE[];
  static const int RIGHT_FACE[];
  static const int LEFT_FACE[];

public:
  AABB();
  virtual ~AABB();

  void SetHalfDimensions(const Vector3 &halfDims);

  void SetUpper(const Vector3 &upper);
  void SetLower(const Vector3 &lower);

  Vector3 GetUpper() const;
  Vector3 GetLower() const;

  bool Intersects(const AABB &other) const;
};