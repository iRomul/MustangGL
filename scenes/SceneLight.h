#pragma once

#include <GL/gl.h>
#include <glm/glm.hpp>

class SceneLight {
    GLenum number;

public:
    SceneLight(GLenum number, glm::vec4 lightAmbient, glm::vec4 lightDiffuse, glm::vec4 specular, glm::vec4 lightPosition);
    void enable();
};
