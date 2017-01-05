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

    glm::vec4 lightAmbient = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);  // Значения фонового свет
    glm::vec4 lightDiffuse = glm::vec4(0.4f, 0.6f, 1.0f, 1.0f);  // Значения диффузного света
    glm::vec4 lightPosition = glm::vec4(0.0f, 4.0f, 0.0f, 1.0f); // Позиция света (w = 1)

    glm::vec3 cameraPos = glm::vec3(6, 6, 6);
    glm::vec3 cameraTarget = glm::vec3(0, 0, 0);

public:
    MainScene(GLFWwindow* window);
    void draw();
    void animate();
    void processKeys();
};
