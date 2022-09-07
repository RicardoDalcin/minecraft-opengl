#include "core/Matrices.hpp"

#include "entity/Camera.hpp"

float g_Ratio = 16.0f / 9.0f;

Camera::Camera(float nearplane = -0.1f, float farplane = -620.0f, float fieldOfView = 3.141592 / 3.0f)
    : m_Nearplane(nearplane),
      m_Farplane(farplane),
      m_FOV(fieldOfView)
{
  updateCameraVectors();
}

void Camera::updatePosition(glm::vec4 newPosition)
{
  m_CameraCenter = newPosition;
}

void Camera::updateCameraAngles(float theta, float phi)
{
  m_CameraTheta = theta;
  m_CameraPhi = phi;

  updateCameraVectors();
}

void Camera::usePerspective()
{
  m_UsePerspective = true;
}

void Camera::useOrthographic()
{
  m_UsePerspective = false;
}

float Camera::getCameraTheta() const
{
  return m_CameraTheta;
}

float Camera::getCameraPhi() const
{
  return m_CameraPhi;
}

glm::vec4 Camera::getPosition() const
{
  return m_CameraCenter;
}

glm::vec4 Camera::getTarget() const
{
  return m_CameraFront;
}

glm::vec4 Camera::getRight() const
{
  return m_CameraRight;
}

glm::mat4 Camera::computeViewMatrix() const
{
  return Matrices::matrixCameraView(m_CameraCenter, m_CameraFront, m_CameraUp);
}

glm::mat4 Camera::computeProjectionMatrix() const
{
  if (m_UsePerspective)
  {
    return Matrices::matrixPerspective(m_FOV, g_Ratio, m_Nearplane, m_Farplane);
  }

  float t = 1.5f * m_CameraDistance / 2.5f;
  float b = -t;
  float r = t * g_Ratio;
  float l = -r;

  return Matrices::matrixOrthographic(l, r, b, t, m_Nearplane, m_Farplane);
}