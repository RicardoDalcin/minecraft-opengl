#include "core.h"

class Bezier
{
private:
  glm::vec2 m_P0;
  glm::vec2 m_P1;
  glm::vec2 m_P2;
  glm::vec2 m_P3;

public:
  Bezier(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3);
  ~Bezier();

  glm::vec2 GetPoint(float pointAt);
};