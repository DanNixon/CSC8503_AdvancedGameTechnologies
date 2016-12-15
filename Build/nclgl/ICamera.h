#pragma once

#include "Keyboard.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "Vector3.h"

/**
 * @class ICamera
 * @author Dan Nixon
 * @brief Interface for a world camera.
 */
class ICamera
{
public:
  static const KeyboardKeys CAMERA_FORWARDS = KeyboardKeys::KEYBOARD_W;
  static const KeyboardKeys CAMERA_BACKWARDS = KeyboardKeys::KEYBOARD_S;
  static const KeyboardKeys CAMERA_LEFT = KeyboardKeys::KEYBOARD_A;
  static const KeyboardKeys CAMERA_RIGHT = KeyboardKeys::KEYBOARD_D;
  static const KeyboardKeys CAMERA_UP = KeyboardKeys::KEYBOARD_SHIFT;
  static const KeyboardKeys CAMERA_DOWN = KeyboardKeys::KEYBOARD_SPACE;

public:
  ICamera();
  virtual ~ICamera();

  /**
   * @brief Gets the flag that defines if mouse and keyboard input is handled by this camera.
   * @return True if MK input is used
   */
  inline bool IsInputEnabled() const
  {
    return m_handleInput;
  }

  /**
   * @brief Sets the flag that defines if mouse and keyboard input is handled by this camera.
   * @param enable True if MK input is used
   */
  void SetHandleInput(bool enable)
  {
    m_handleInput = enable;
  }

  virtual void HandleMouse(float dt);

  /**
   * @brief Handle keyboard input
   * @param dt Time in milliseconds since last update
   */
  virtual void HandleKeyboard(float dt)
  {
  }

  /**
   * @brief Builds a view matrix for graphic rendering.
   * @return View matrix
   */
  virtual Matrix4 BuildViewMatrix() = 0;

  /**
   * @brief Gets the Cartesian world space coordinates.
   * @return World space position
   */
  virtual Vector3 GetPosition() const = 0;

  virtual Quaternion GetOrientation() const;

  /**
   * @brief Gets yaw angle in dgrees.
   * @return Yaw angle
   */
  float GetYaw() const
  {
    return m_yaw;
  }

  /**
   * @brief Sets yaw angle in dgrees.
   * @param y Yaw angle
   */
  void SetYaw(float y)
  {
    m_yaw = y;
  }

  /**
   * @brief Gets pitch angle in dgrees.
   * @return Pitch angle
   */
  float GetPitch() const
  {
    return m_pitch;
  }

  /**
   * @brief Sets yaw angle in dgrees.
   * @param p Pitch angle
   */
  void SetPitch(float p)
  {
    m_pitch = p;
  }

protected:
  bool m_handleInput; //!< Flag indicating if input should be handled

  float m_pitch; //!< Pitch view angle
  float m_yaw;   //!< Yaw view angle
};