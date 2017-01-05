#include "RoadModel.h"

#include <iostream>

using namespace std;

RoadModel::RoadModel() :
        groundTexture("ground.bmp") {

}

void RoadModel::load() {
    if(!groundTexture.load()) {
        cerr << "Can't load texture bitmap file" << endl;
        exit(EXIT_FAILURE);
    }
}

void RoadModel::draw() {
    groundTexture.bind();
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 10.0f);
    glVertex3f(-1.0f, 0.0f, -1.0f); //10 - текстуры тайлятся
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, 0.0f, 1.0f);
    glTexCoord2f(10.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 1.0f);
    glTexCoord2f(10.0f, 10.0f);
    glVertex3f(1.0f, 0.0f, -1.0f);
    glEnd();
}
