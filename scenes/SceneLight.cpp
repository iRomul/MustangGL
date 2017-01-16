#include "SceneLight.h"

#include <glm/ext.hpp>

using namespace glm;

SceneLight::SceneLight(GLenum number, vec4 ambient, vec4 diffuse, vec4 specular, vec4 lightPosition) {
    this->number = number;

    glLightfv(number, GL_AMBIENT, &ambient[0]);
    glLightfv(number, GL_DIFFUSE, &diffuse[0]);
    glLightfv(number, GL_SPECULAR, &specular[0]);
    glLightfv(number, GL_POSITION, value_ptr(lightPosition));
}

void SceneLight::enable() {
    glEnable(number);
}
