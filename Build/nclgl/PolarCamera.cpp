#include "PolarCamera.h"

#include "Window.h"

/**
 * @brief Creates a new camera in PolarCamera coordinate space.
 */
PolarCamera::PolarCamera()
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

  float speed = 3.5f * dt; // 3.5m per second

  // TODO
#if 0
  if (Window::GetKeyboard()->KeyDown(CAMERA_FORWARDS))
    m_position += Matrix4::Rotation(m_yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * speed;

  if (Window::GetKeyboard()->KeyDown(CAMERA_BACKWARDS))
    m_position -= Matrix4::Rotation(m_yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * speed;

  if (Window::GetKeyboard()->KeyDown(CAMERA_LEFT))
    m_position += Matrix4::Rotation(m_yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * speed;

  if (Window::GetKeyboard()->KeyDown(CAMERA_RIGHT))
    m_position -= Matrix4::Rotation(m_yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * speed;
#endif

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
  // TODO
  return Matrix4();
}

Vector3 PolarCamera::GetPosition() const
{
  // TODO
  return Vector3();
}