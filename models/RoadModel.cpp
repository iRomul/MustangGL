#include "RoadModel.h"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

using namespace std;

RoadModel::RoadModel() :
        groundTexture("ground.bmp") {

}

void RoadModel::load() {
    if (!groundTexture.load()) {
        cerr << "Can't load texture bitmap file" << endl;
        exit(EXIT_FAILURE);
    }
}

void RoadModel::draw() {
    groundTexture.bind();

    glm::vec4 ambient(0.2f, 0.2f, 0.2f, 0.35f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glm::value_ptr(ambient));
    glm::vec4 diffuse(0.8f, 0.8f, 0.8f, 0.35f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm::value_ptr(diffuse));
    glm::vec4 specular(0.5f, 0.5f, 0.5f, 0.35f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr(specular));
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 10.0f);
    glVertex3f(-3.0f, 0.0f, -3.0f); //10 - текстуры тайлятся
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-3.0f, 0.0f, 3.0f);
    glTexCoord2f(10.0f, 0.0f);
    glVertex3f(3.0f, 0.0f, 3.0f);
    glTexCoord2f(10.0f, 10.0f);
    glVertex3f(3.0f, 0.0f, -3.0f);
    glEnd();
}
