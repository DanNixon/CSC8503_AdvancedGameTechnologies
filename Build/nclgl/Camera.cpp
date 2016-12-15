#include "Camera.h"

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/
void Camera::HandleMouse(float dt)
{
  if (!m_handleInput)
    return;

  // Update the mouse by how much
  if (Window::GetMouse()->ButtonDown(MOUSE_LEFT))
  {
    m_pitch -= (Window::GetMouse()->GetRelativePosition().y);
    m_yaw -= (Window::GetMouse()->GetRelativePosition().x);
  }

  // Bounds check the pitch, to be between straight up and straight down ;)
  m_pitch = min(m_pitch, 90.0f);
  m_pitch = max(m_pitch, -90.0f);

  if (m_yaw < 0)
    m_yaw += 360.0f;

  if (m_yaw > 360.0f)
    m_yaw -= 360.0f;
}

void Camera::HandleKeyboard(float dt)
{
  if (!m_handleInput)
    return;

  float speed = 3.5f * dt; // 3.5m per second

  // Bounds check the pitch, to be between straight up and straight down ;)
  if (m_yaw < 0)
    m_yaw += 360.0f;

  if (m_yaw > 360.0f)
    m_yaw -= 360.0f;

  if (Window::GetKeyboard()->KeyDown(KEYBOARD_W))
    m_position += Matrix4::Rotation(m_yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * speed;

  if (Window::GetKeyboard()->KeyDown(KEYBOARD_S))
    m_position -= Matrix4::Rotation(m_yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * speed;

  if (Window::GetKeyboard()->KeyDown(KEYBOARD_A))
    m_position += Matrix4::Rotation(m_yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * speed;

  if (Window::GetKeyboard()->KeyDown(KEYBOARD_D))
    m_position -= Matrix4::Rotation(m_yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * speed;

  if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT))
    m_position.y += speed;

  if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE))
    m_position.y -= speed;
}

/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
Matrix4 Camera::BuildViewMatrix()
{
  // Why do a complicated matrix inversion, when we can just generate the matrix
  // using the negative values ;). The matrix multiplication order is important!
  return Matrix4::Rotation(-m_pitch, Vector3(1, 0, 0)) * Matrix4::Rotation(-m_yaw, Vector3(0, 1, 0)) *
         Matrix4::Translation(-m_position);
}

Quaternion Camera::GetOrientation() const
{
  return Quaternion::EulerAnglesToQuaternion(m_pitch, m_yaw, 0.0f);
}
