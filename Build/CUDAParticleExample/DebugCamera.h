#pragma once
#include "Matrix4.h"
#include <GL/glew.h>

class DebugCamera
{
public:
  DebugCamera(Vector3 pos, float azimuth, float altitude)
      : pos(pos)
      , azimuth(azimuth)
      , altitude(altitude)
      , az_speed(1)
      , al_speed(1)
      , mv_speed(3)
      , plusdown(false)
      , minusdown(false)
  {
    updateView();
  }

  void updateView();
  Matrix4 getTransformMatrix() const
  {
    return transform;
  }

  void setMovementSpeed(float speed)
  {
    mv_speed = speed;
  }

protected:
  void updateFromInput();
  Matrix4 getRotationMatrix();

  Matrix4 transform;

  Vector3 pos;
  float az_speed, al_speed, mv_speed;
  float azimuth, altitude;

  bool plusdown, minusdown;
};