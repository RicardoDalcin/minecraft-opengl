#include "core/Matrices.hpp"

#include "entity/Camera.hpp"

float g_Ratio = 16.0f / 9.0f;

Camera::Camera(float nearplane = -0.1f, float farplane = -620.0f, float fieldOfView = 60.0f)
    : m_Nearplane(nearplane),
      m_Farplane(farplane),
      m_FOV(fieldOfView)
{
  UpdateCameraVectors();
}

void Camera::UpdatePosition(glm::vec4 newPosition)
{
  m_CameraCenter = newPosition;
}

void Camera::UpdateCameraAngles(float theta, float phi)
{
  m_CameraTheta = theta;
  m_CameraPhi = phi;

  UpdateCameraVectors();
}

void Camera::UsePerspective()
{
  m_UsePerspective = true;
}

void Camera::UseOrthographic()
{
  m_UsePerspective = false;
}

void Camera::UseFreeCamera()
{
  m_UseFreeCamera = true;
}

void Camera::UseLookAtCamera()
{
  m_UseFreeCamera = false;
}

float Camera::GetCameraTheta() const
{
  return m_CameraTheta;
}

float Camera::GetCameraPhi() const
{
  return m_CameraPhi;
}

glm::vec4 Camera::GetPosition() const
{
  return m_CameraCenter;
}

glm::vec4 Camera::GetTarget() const
{
  return m_CameraFront;
}

glm::vec4 Camera::GetRight() const
{
  return m_CameraRight;
}

glm::mat4 Camera::ComputeViewMatrix() const
{
  if (m_UseFreeCamera)
  {
    return Matrices::MatrixCameraView(m_CameraCenter, m_CameraFront, m_CameraUp);
  }

  float r = 2.5f;
  float y = r * sin(m_CameraPhi);
  float z = r * cos(m_CameraPhi) * cos(m_CameraTheta);
  float x = r * cos(m_CameraPhi) * sin(m_CameraTheta);

  glm::vec4 cameraPosition = glm::vec4(m_CameraCenter.x - x, m_CameraCenter.y - y, m_CameraCenter.z - z, 1.0f);
  glm::vec4 lookAtPosition = glm::vec4(m_CameraCenter.x, m_CameraCenter.y, m_CameraCenter.z, 1.0f);
  glm::vec4 viewVector = lookAtPosition - cameraPosition;

  return Matrices::MatrixCameraView(cameraPosition, viewVector, m_CameraUp);
}

glm::mat4 Camera::ComputeProjectionMatrix() const
{
  if (m_UsePerspective)
  {
    return Matrices::MatrixPerspective(m_FOV * 3.141592f / 180.0f, g_Ratio, m_Nearplane, m_Farplane);
  }

  float t = 1.5f * m_CameraDistance / 2.5f;
  float b = -t;
  float r = t * g_Ratio;
  float l = -r;

  return Matrices::MatrixOrthographic(l, r, b, t, m_Nearplane, m_Farplane);
}
