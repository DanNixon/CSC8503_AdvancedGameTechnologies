#pragma once

#include "ICamera.h"

/**
 * @class CartesianCamera
 * @brief A camera that operates in Cartesian coordinates.
 */
class CartesianCamera : public ICamera
{
public:
  CartesianCamera(float pitch, float yaw, Vector3 position);
  virtual ~CartesianCamera();

  virtual void HandleKeyboard(float dt) override;

  virtual Matrix4 BuildViewMatrix() override;

  /**
   * @copydoc ICamera::GetPosition
   */
  virtual Vector3 GetPosition() const override
  {
    return m_position;
  }

  /**
   * @brief Sets position in world space.
   * @param val World space position
   */
  void SetPosition(Vector3 val)
  {
    m_position = val;
  }

protected:
  Vector3 m_position; //!< World space position
};