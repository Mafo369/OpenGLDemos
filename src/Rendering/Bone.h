#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "RenderObject.h"

class Bone {
public:
    Bone(glm::vec3 p1, glm::vec3 p2, std::shared_ptr<Material> material);
    Bone(glm::mat4 p);

    void updateMesh();

    void rotate(glm::mat4 rot);
    void rotateRestPose(float angle, glm::vec3 axis);
    void rotate(float angle, glm::vec3 axis);

    void savePose();
    void setPosition(glm::vec3 pos);

    glm::mat4 getBlendPose();

    glm::vec3 m_p1;
    glm::vec3 m_p2;

    glm::mat4 m_restPose;
    glm::mat4 m_pose;
    std::vector<Bone*> m_childs;
    RenderObject* m_object;

private:

};
