#pragma once

#include "models/RoadModel.h"
#include "models/VehicleModel.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

enum ViewMode {
    TOP,//вид сверху
    OBSERVE,//вид откуда-то с башни
    FIXED_OBSERVE,//вид как в РПГшке
};

enum AnimationState {
    DOOR_CLOSED,
    DOOR_OPENING,
    DOOR_OPENED,
    DOOR_CLOSING,
};

class MainScene {
    GLFWwindow* window;

    VehicleModel model;
    RoadModel road;

    glm::vec3 cameraPos = glm::vec3(6, 6, 6);
    glm::vec3 cameraTarget = glm::vec3(0, 0, 0);

public:
    MainScene(GLFWwindow* window);
    void draw();
    void animate();
    void processKeys();
};
