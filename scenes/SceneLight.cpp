#include "SceneLight.h"

#include <glm/ext.hpp>

using namespace glm;

SceneLight::SceneLight(GLenum number, vec4 lightAmbient, vec4 lightDiffuse, vec4 lightPosition) {
    this->number = number;

    glLightfv(number, GL_AMBIENT, &lightAmbient[0]);
    glLightfv(number, GL_DIFFUSE, &lightDiffuse[0]);
    glLightfv(number, GL_POSITION, value_ptr(lightPosition));
}

void SceneLight::enable() {
    glEnable(number);
}
