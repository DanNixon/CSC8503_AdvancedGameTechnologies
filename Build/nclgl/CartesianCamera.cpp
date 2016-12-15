#include "CartesianCamera.h"

#include "Window.h"

/**
 * @brief Creates a new camera in Cartesian coordinate space.
 * @param pitch Pitch viewing angle
 * @param yaw Yaw viewing angle
 * @param position Cartesian world space position
 */
CartesianCamera::CartesianCamera(float pitch, float yaw, Vector3 position)
{
  m_pitch = m_pitch;
  m_yaw = m_yaw;
  m_position = position;
}

CartesianCamera::~CartesianCamera()
{
}

/**
 * @copydoc ICamera::HandleKeyboard
 */
void CartesianCamera::HandleKeyboard(float dt)
{
  if (!m_handleInput)
    return;

  float speed = 3.5f * dt; // 3.5m per second

  // Bounds check the pitch, to be between straight up and straight down ;)
  if (m_yaw < 0)
    m_yaw += 360.0f;

  if (m_yaw > 360.0f)
    m_yaw -= 360.0f;

  if (Window::GetKeyboard()->KeyDown(CAMERA_FORWARDS))
    m_position += Matrix4::Rotation(m_yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * speed;

  if (Window::GetKeyboard()->KeyDown(CAMERA_BACKWARDS))
    m_position -= Matrix4::Rotation(m_yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * speed;

  if (Window::GetKeyboard()->KeyDown(CAMERA_LEFT))
    m_position += Matrix4::Rotation(m_yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * speed;

  if (Window::GetKeyboard()->KeyDown(CAMERA_RIGHT))
    m_position -= Matrix4::Rotation(m_yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * speed;

  if (Window::GetKeyboard()->KeyDown(CAMERA_UP))
    m_position.y += speed;

  if (Window::GetKeyboard()->KeyDown(CAMERA_DOWN))
    m_position.y -= speed;
}

/**
 * @copydoc ICamera::BuildViewMatrix
 */
Matrix4 CartesianCamera::BuildViewMatrix()
{
  // Why do a complicated matrix inversion, when we can just generate the matrix
  // using the negative values ;). The matrix multiplication order is important!
  return Matrix4::Rotation(-m_pitch, Vector3(1, 0, 0)) * Matrix4::Rotation(-m_yaw, Vector3(0, 1, 0)) *
         Matrix4::Translation(-m_position);
}
