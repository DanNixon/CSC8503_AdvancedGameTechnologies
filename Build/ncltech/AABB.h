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

  inline Vector3 GetUpper() const
  {
    return Vector3(m_vVertices[6].pos.x, m_vVertices[6].pos.y, m_vVertices[6].pos.z);
  }

  inline Vector3 GetLower() const
  {
    return Vector3(m_vVertices[0].pos.x, m_vVertices[0].pos.y, m_vVertices[0].pos.z);
  }

  AABB Transform(const Matrix4 &transformation) const;

  bool Intersects(const AABB &other) const;
};