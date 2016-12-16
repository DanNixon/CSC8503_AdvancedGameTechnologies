#pragma once

#include "ICamera.h"

/**
 * @class PolarCamera
 * @brief A camera that operates in Polar coordinates.
 */
class PolarCamera : public ICamera
{
public:
  PolarCamera();
  virtual ~PolarCamera();

  virtual void HandleKeyboard(float dt) override;

  virtual Matrix4 BuildViewMatrix() override;
  virtual Vector3 GetPosition() const override;
  virtual Quaternion GetOrientation() const override;

  void SetDistance(float dist)
  {
    m_distance = dist;
  }

protected:
  Vector3 m_origin;                //!< Origin point in Cartesian coordinate space
  Quaternion m_positionalRotation; //!< ROtation about otigin point
  float m_distance;                //!< DIstance from origin point
};