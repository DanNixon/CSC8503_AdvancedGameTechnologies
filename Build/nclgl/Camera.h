/******************************************************************************
Class:Camera
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description:FPS-Style camera. Uses the mouse and keyboard from the Window
class to get movement values!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*/ /////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Matrix4.h"
#include "Vector3.h"
#include "Window.h"

class Camera
{
public:
  Camera(void)
  {
    m_position = Vector3(0.0f, 0.0f, 0.0f);
    m_yaw = 0.0f;
    m_pitch = 0.0f;
  };

  Camera(float m_pitch, float m_yaw, Vector3 position)
  {
    this->m_pitch = m_pitch;
    this->m_yaw = m_yaw;
    this->m_position = position;
  }

  ~Camera(void){};

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

  void HandleMouse(float dt);
  void HandleKeyboard(float dt);

  // Builds a view matrix for the current camera variables, suitable for sending straight
  // to a vertex shader (i.e it's already an 'inverse camera matrix').
  Matrix4 BuildViewMatrix();

  // Gets position in world space
  Vector3 GetPosition() const
  {
    return m_position;
  }

  // Sets position in world space
  void SetPosition(Vector3 val)
  {
    m_position = val;
  }

  // Gets yaw, in degrees
  float GetYaw() const
  {
    return m_yaw;
  }

  // Sets yaw, in degrees
  void SetYaw(float y)
  {
    m_yaw = y;
  }

  // Gets pitch, in degrees
  float GetPitch() const
  {
    return m_pitch;
  }

  // Sets pitch, in degrees
  void SetPitch(float p)
  {
    m_pitch = p;
  }

  Quaternion GetOrientation() const;

protected:
  bool m_handleInput;
  float m_yaw;
  float m_pitch;
  Vector3 m_position;
};