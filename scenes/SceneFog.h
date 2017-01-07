#pragma once

#include <gl/gl.h>
#include <glm/glm.hpp>

class SceneFog {
public:
    SceneFog(glm::vec4 color, float density = 0.35f, GLenum fogMode = GL_LINEAR);
    void enable();
};
