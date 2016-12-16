#pragma once

#include "ICamera.h"

/**
 * @class PolarCamera
 * @author Dan Nixon
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

  /**
   * @brief Gets the origin position.
   * @return Origin
   */
  inline Vector3 GetOrigin() const
  {
    return m_origin;
  }

  /**
   * @brief Sets the origin position.
   * @param origin Origin
   */
  void SetOrigin(const Vector3 &origin)
  {
    m_origin = origin;
  }

  /**
   * @brief Gets rotation about the centre point.
   * @return Positional rotation
   */
  inline Quaternion GetPositionalRotation() const
  {
    return m_positionalRotation;
  }

  /**
   * @brief Gets distance from origin point.
   * @retrun Distance
   */
  inline float GetDistance() const
  {
    return m_distance;
  }

  /**
   * @brief Sets distance from origin point.
   * @param dist Distance
   */
  void SetDistance(float dist)
  {
    m_distance = dist;
  }

protected:
  Vector3 m_origin;                //!< Origin point in Cartesian coordinate space
  Quaternion m_positionalRotation; //!< ROtation about otigin point
  float m_distance;                //!< DIstance from origin point
};