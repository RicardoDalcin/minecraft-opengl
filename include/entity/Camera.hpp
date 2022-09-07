#ifndef _CAMERA_H
#define _CAMERA_H

#include <glm/glm.hpp>

#include "core/Matrices.hpp"

class Camera
{
private:
  float m_CameraTheta = 3.141592f / 4; // Ângulo no plano ZX em relação ao eixo Z
  float m_CameraPhi = -3.141592f / 6;  // Ângulo em relação ao eixo Y
  float m_CameraDistance = 160.0f;     // Distância da câmera para a origem

  bool m_UsePerspective = true;

  glm::vec4 m_CameraCenter = glm::vec4(0.0f, 64.0f, -3.0f, 1.0f);
  glm::vec4 m_CameraFront;
  glm::vec4 m_CameraRight;

  const glm::vec4 m_CameraUp = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

  const float m_Nearplane;
  const float m_Farplane;
  const float m_FOV;

  void updateCameraVectors()
  {
    float x = cos(m_CameraPhi) * sin(m_CameraTheta);
    float y = sin(m_CameraPhi);
    float z = cos(m_CameraPhi) * cos(m_CameraTheta);

    m_CameraFront = glm::normalize(glm::vec4(x, y, z, 0.0f));

    glm::vec4 w = -m_CameraFront / Matrices::norm(m_CameraFront);

    m_CameraRight = Matrices::crossproduct(m_CameraUp, w) / Matrices::norm(Matrices::crossproduct(m_CameraUp, w));
  };

public:
  Camera(float nearplane, float farplane, float fieldOfView);
  ~Camera() {}

  void updatePosition(glm::vec4 newPosition);
  void updateCameraAngles(float theta, float phi);

  void usePerspective();
  void useOrthographic();

  float getCameraTheta() const;
  float getCameraPhi() const;

  glm::vec4 getPosition() const;
  glm::vec4 getTarget() const;
  glm::vec4 getRight() const;

  glm::mat4 computeViewMatrix() const;
  glm::mat4 computeProjectionMatrix() const;
};

#endif