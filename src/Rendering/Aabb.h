#include <glm/glm.hpp>

class Aabb {
public:
  Aabb(glm::vec3 min, glm::vec3 max);
  ~Aabb();

  glm::vec3 min() { return m_min; }
  glm::vec3 max() { return m_max; }

  glm::vec3 m_min, m_max;
};
