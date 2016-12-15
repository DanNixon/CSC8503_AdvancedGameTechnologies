#include "ICamera.h"

#include "Window.h"

/**
 * @brief Creates a new camera.
 */
ICamera::ICamera()
    : m_handleInput(true)
    , m_pitch(0.0f)
    , m_yaw(0.0f)
{
}

ICamera::~ICamera()
{
}

/**
 * @brief Handle mouse input
 * @param dt Time in milliseconds since last update
 */
void ICamera::HandleMouse(float dt)
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

/**
 * @brief Gets a quaternion describing the direction the camera is looking.
 * @return Look direction
 */
Quaternion ICamera::GetOrientation() const
{
  return Quaternion::EulerAnglesToQuaternion(m_pitch, m_yaw, 0.0f);
}
