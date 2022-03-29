#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

typedef struct s_Attenuation
{
    float constant = 1;
    float linear = 0;
    float quadratic = 0;
}Attenuation;

typedef struct s_Light
{
    glm::vec3 position;
    glm::vec3 color; // Colour
    Attenuation attenuation;
}Light;

#endif
