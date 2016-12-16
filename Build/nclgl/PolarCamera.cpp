#include "PolarCamera.h"

#include "Window.h"

/**
 * @brief Creates a new camera in PolarCamera coordinate space.
 */
PolarCamera::PolarCamera()
    : m_origin(0.0f, 0.0f, 0.0f)
    , m_distance(0.0f)
{
}

PolarCamera::~PolarCamera()
{
}

/**
 * @copydoc ICamera::HandleKeyboard
 */
void PolarCamera::HandleKeyboard(float dt)
{
  if (!m_handleInput)
    return;

  float speed = m_speed * dt;

  Matrix4 orientation = GetOrientation().ToMatrix4();
  Vector3 originalPosition = GetPosition();
  Vector3 targetPosition = originalPosition;
  bool targetPosChanged = false;

  if (Window::GetKeyboard()->KeyDown(CAMERA_FORWARDS))
  {
    targetPosition += -orientation.GetBackVector() * speed;
    targetPosChanged = true;
  }

  if (Window::GetKeyboard()->KeyDown(CAMERA_BACKWARDS))
  {
    targetPosition += orientation.GetBackVector() * speed;
    targetPosChanged = true;
  }

  if (Window::GetKeyboard()->KeyDown(CAMERA_LEFT))
  {
    targetPosition += -orientation.GetRightVector() * speed;
    targetPosChanged = true;
  }

  if (Window::GetKeyboard()->KeyDown(CAMERA_RIGHT))
  {
    targetPosition += orientation.GetRightVector() * speed;
    targetPosChanged = true;
  }

  if (targetPosChanged)
  {
    m_positionalRotation = m_positionalRotation * Quaternion::FromVectors(originalPosition, targetPosition);
  }

  if (Window::GetKeyboard()->KeyDown(CAMERA_UP))
    m_distance += speed;

  if (Window::GetKeyboard()->KeyDown(CAMERA_DOWN))
    m_distance -= speed;
}

/**
 * @copydoc ICamera::BuildViewMatrix
 */
Matrix4 PolarCamera::BuildViewMatrix()
{
  return Matrix4::Rotation(-m_pitch, Vector3(1, 0, 0)) * Matrix4::Rotation(-m_yaw, Vector3(0, 1, 0)) *
         Matrix4::Translation(Vector3(0.0f, -m_distance, 0.0f)) * m_positionalRotation.Inverse().ToMatrix4();
}

/**
 * @copydoc ICamera::GetPosition
 */
Vector3 PolarCamera::GetPosition() const
{
  Vector3 offset(0.0f, m_distance, 0.0f);
  Quaternion::RotatePointByQuaternion(m_positionalRotation, offset);
  offset += m_origin;
  return offset;
}