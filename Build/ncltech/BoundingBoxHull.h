#pragma once

#include "BoundingBox.h"
#include "Hull.h"

/**
 * @class BoundingBoxHull
 * @author Dan Nixon
 * @brief Bounding box that is synchronised with a cuboid hull.
 */
class BoundingBoxHull : public BoundingBox, public Hull
{
public:
  static const int FAR_FACE[];
  static const int NEAR_FACE[];
  static const int TOP_FACE[];
  static const int BOTTOM_FACE[];
  static const int RIGHT_FACE[];
  static const int LEFT_FACE[];

public:
  BoundingBoxHull();
  virtual ~BoundingBoxHull();

  void UpdateHull();
};
