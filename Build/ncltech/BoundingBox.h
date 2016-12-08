#pragma once

#include "Hull.h"

class BoundingBox : public Hull
{
public:
  static const int FAR_FACE[];
  static const int NEAR_FACE[];
  static const int TOP_FACE[];
  static const int BOTTOM_FACE[];
  static const int RIGHT_FACE[];
  static const int LEFT_FACE[];

public:
  BoundingBox(bool generateHull = true);
  virtual ~BoundingBox();

  void ExpandToFit(const Vector3 &point);
  void SetHalfDimensions(const Vector3 &halfDims);

  Vector3 &Lower()
  {
    return m_lower;
  }

  const Vector3 &Lower() const
  {
    return m_lower;
  }

  Vector3 &Upper()
  {
    return m_upper;
  }

  const Vector3 &Upper() const
  {
    return m_upper;
  }

  BoundingBox Transform(const Matrix4 &transformation) const;

  bool Intersects(const BoundingBox &other) const;

  void UpdateHull();

protected:
  Vector3 m_lower; //!< Lower vertex of bounding box
  Vector3 m_upper; //!< Upper vertex of bounding box
};