#include "Bone.h"
#include <GL/gl.h>
#include <glm/gtc/matrix_transform.hpp>

Bone::Bone(glm::vec3 p1, glm::vec3 p2, std::shared_ptr<Material> material) {

    m_restPose = glm::mat4(1.);
    m_pose = glm::mat4(1.);

    m_p1 = p1;
    m_p2 = p2;

    Vertex p1V = {p1, {0,0,1}, {0,0}, {1,0,0,1}, {0,0,0}, {0,0,0}};
    Vertex p2V = {p2, {0,0,1}, {0,0}, {1,0,0,1}, {0,0,0}, {0,0,0}};
    Mesh* m = new Mesh({p1V, p2V}, {0, 1}, GL_LINES);
    m_object = new RenderObject(m, material);
}

Bone::Bone(glm::mat4 p) {
    m_restPose = p;
    m_pose = p;
}

void Bone::updateMesh() {
    Vertex p1V = {getBlendPose()*glm::vec4(m_p1, 1), {0,0,1}, {0,0}, {1,0,0,1}, {0,0,0}, {0,0,0}};
    Vertex p2V = {getBlendPose()*glm::vec4(m_p2, 1), {0,0,1}, {0,0}, {1,0,0,1}, {0,0,0}, {0,0,0}};
    Mesh* m = new Mesh({p1V, p2V}, {0, 1}, GL_LINES);
    m_object->setMesh(m);
}

void Bone::rotateRestPose(float angle, glm::vec3 axis) {
    m_pose = glm::rotate(m_restPose, glm::radians(angle), axis);
    updateMesh();
    for(auto i:m_childs){
        i->rotate(getBlendPose());
    }
}

void Bone::rotate(float angle, glm::vec3 axis) {
    m_pose = glm::rotate(m_pose, glm::radians(angle), axis);
    updateMesh();
    for(auto c : m_childs){
        c->rotate(getBlendPose());
    }
}

void Bone::rotate(glm::mat4 rot) {
    m_pose = rot * m_pose;
    updateMesh();
}

void Bone::savePose() {
    m_restPose = m_pose;
}

void Bone::setPosition(glm::vec3 pos) {
    m_pose[3][0] = pos.x;
    m_pose[3][1] = pos.y;
    m_pose[3][2] = pos.z;
    m_restPose[3][0] = pos.x;
    m_restPose[3][1] = pos.y;
    m_restPose[3][2] = pos.z;
}

glm::mat4 Bone::getBlendPose() {
    return m_pose * glm::inverse(m_restPose);
}
