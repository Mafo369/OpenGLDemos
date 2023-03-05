#include "VerletParticle.h"
#include "../../Rendering/Renderer.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/string_cast.hpp>

VerletParticle::VerletParticle(Mesh* mesh, std::shared_ptr<Material> material, glm::vec3 position, bool lock)
  : RenderObject(mesh, material) {

    m_position = position; // Current position
    m_prev = position; // Previous position
    m_initPos = position; // Initial position - Necessary for translation
    m_transform = glm::translate(glm::mat4(1.f), m_initPos);
    m_mass = 1.f; // 1kg
    m_drag = 0.999; // rho * C * Area - simplified drag for this example
    m_force = glm::vec3(0.f, 0.f, 0.f);
    m_temp = glm::vec3(0.f, 0.f, 0.f);
    m_strength = 0.5f; // Strength of springs
    m_isLocked = lock;
}

VerletParticle::~VerletParticle() {

}

void VerletParticle::addConstraint(Aabb c) {
    m_constraints.push_back(c);    
}

void VerletParticle::updateConstraints( glm::vec3 v ) {
    for( auto c : m_constraints ) {
        if ( m_position.x > c.max().x ) {
            m_position = glm::vec3( c.max().x, m_position.y, m_position.z );
            m_prev = glm::vec3( m_position.x + v.x, m_prev.y, m_prev.z );
        } else if ( m_position.x < c.min().x ) {
            m_position = glm::vec3( c.min().x, m_position.y, m_position.z );
            m_prev = glm::vec3( m_position.x + v.x, m_prev.y, m_prev.z );
        }
        if ( m_position.y > c.max().y ) {
            m_position = glm::vec3( m_position.x, c.max().y, m_position.z );
            m_prev = glm::vec3( m_prev.x, m_position.y + v.y, m_prev.z );
        } else if ( m_position.y < c.min().y ) {
            m_position = glm::vec3( m_position.x, c.min().y, m_position.z );
            m_prev = glm::vec3( m_prev.x, m_position.y + v.y, m_prev.z );
        }
        if ( m_position.z > c.max().z ) {
            m_position = glm::vec3( m_position.x, m_position.y, c.max().z );
            m_prev = glm::vec3( m_prev.x, m_prev.y, m_position.z + v.z );
        } else if ( m_position.z < c.min().z ) {
            m_position = glm::vec3( m_position.x, m_position.y, c.min().z );
            m_prev = glm::vec3( m_prev.x, m_prev.y, m_position.z + v.z );
        }
    }
}

void VerletParticle::update() {
    if( !m_isLocked ){
        glm::vec3 v = ( m_position - m_prev ) * m_drag; // Velocity
        m_prev = m_position;
        m_position = m_position + v;

        for( auto b : behaviors ) {
            m_position = m_position + b;
        }
        //for( auto a : attractionBehaviors ) {
        //    auto delta = a.first - position;
        //    float dist = delta.x() * delta.x() + delta.y() * delta.y() + delta.z() * delta.z();
        //    float radiusSq = a.second.first * a.second.first;
        //    if(dist < radiusSq){
        //        auto f = scale( normalizeTo( delta, 1.f - dist / radiusSq ), a.second.second * t );
        //        position = position + f;
        //    }
        //}
        //for( auto a : repulsionBehaviors ) {
        //    auto delta = a.first - position;
        //    float dist = delta.x() * delta.x() + delta.y() * delta.y() + delta.z() * delta.z();
        //    float radiusSq = a.second.first * a.second.first;
        //    if(dist < radiusSq){
        //        auto f = scale( normalizeTo( delta, 1.f - dist / radiusSq ), a.second.second * t );
        //        position = position - f;
        //    }
        //}
        updateConstraints( v );
        m_transform = glm::translate(glm::mat4(1.f), m_initPos + (m_position - m_initPos));
    }
}

void VerletParticle::addBehavior(glm::vec3 b) {
  behaviors.push_back(b);
}
