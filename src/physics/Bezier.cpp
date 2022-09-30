#include "physics/Bezier.hpp"

Bezier::Bezier(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
    : m_P0(p0),
      m_P1(p1),
      m_P2(p2),
      m_P3(p3)
{
}

Bezier::~Bezier()
{
}

glm::vec2 Bezier::GetPoint(float pointAt)
{
  glm::vec2 p01 = m_P0 + pointAt * (m_P1 - m_P0);
  glm::vec2 p12 = m_P1 + pointAt * (m_P2 - m_P1);
  glm::vec2 p23 = m_P2 + pointAt * (m_P3 - m_P2);

  glm::vec2 p012 = p01 + pointAt * (p12 - p01);
  glm::vec2 p123 = p12 + pointAt * (p23 - p12);

  glm::vec2 p0123 = p012 + pointAt * (p123 - p012);

  return p0123;
}