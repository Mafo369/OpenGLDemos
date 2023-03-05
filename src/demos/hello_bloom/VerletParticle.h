#include "../../Rendering/RenderObject.h"
#include "../../Rendering/Aabb.h"

//typedef struct s_Aabb {
//  glm::vec3 m_min, m_max;
//  glm::vec3 min() { return m_min; }
//  glm::vec3 max() { return m_max; }
//} Aabb;

class VerletParticle : public RenderObject {
  public:
    VerletParticle(Mesh* mesh, std::shared_ptr<Material> material, glm::vec3 position, bool lock);
    ~VerletParticle();

    void addConstraint(Aabb c);

    void addBehavior( glm::vec3 b ); // Constant behavior

    void update();
    void updateConstraints(glm::vec3 velocity);

    glm::vec3 m_initPos, m_position, m_prev, m_temp, m_force;
    float m_mass, m_drag, m_strength, m_r;
    bool m_isLocked;

    std::vector<Aabb> m_constraints;

    /// Constant Behaviors list
    std::vector<glm::vec3> behaviors;

};
