#include "MainScene.h"

#include <glm/ext.hpp>

#include "math_util.h"

using namespace std;
using namespace glm;

ViewMode viewMode = OBSERVE;
AnimationState animState = DOOR_CLOSED;

MainScene::MainScene(GLFWwindow* window) {
    this->window = window;

    glLightfv(GL_LIGHT1, GL_AMBIENT, &lightAmbient[0]);        // установим Ambient Light
    glLightfv(GL_LIGHT1, GL_DIFFUSE, &lightDiffuse[0]);        // установим Diffuse Light
    glLightfv(GL_LIGHT1, GL_POSITION, glm::value_ptr(lightPosition));    // установим позицию The Light
    glEnable(GL_LIGHT1);                                // включим

    vec3 cameraPos = vec3(6, 6, 6);
    vec3 cameraTarget = vec3(0, 0, 0);

    model.load();
    road.load();
}

void MainScene::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);

    int w, h;

    glfwGetWindowSize(window, &w, &h);

    GLfloat aspectRatio = (GLfloat) w / (GLfloat) h;

    if (viewMode == OBSERVE) {
        mat4 projectionMatrix =
                perspective(45.0f, aspectRatio, 0.1f, 100.0f) * lookAt(cameraPos, cameraTarget, vec3(0, 1, 0));
        glLoadMatrixf(glm::value_ptr(projectionMatrix));
    } else if (viewMode == TOP) {
        mat4 projectionMatrix = perspective(45.0f, aspectRatio, 0.1f, 100.0f) *
                                lookAt(vec3(model.modelPos.x, 20.0f, model.modelPos.z - 2), model.modelPos, vec3(0, 0, 1));
        glLoadMatrixf(glm::value_ptr(projectionMatrix));
    } else if (viewMode == FIXED_OBSERVE) {
        //берем матрицу модели
        mat4 modelMatrix = mat4(1);
        modelMatrix = translate(modelMatrix, model.modelPos);
        modelMatrix = rotate(modelMatrix, degreesToRadians(model.modelRot), vec3(0, 1, 0));

        //устанавливаем камеру относительно модели и получим ее мировые координаты
        //vec3 cameraPos = modelMatrix * vec4(0.0, 5.0, 8.0, 1.0); //w будет 1

        mat4 perspectiveMatrix = perspective(
                45.0f,
                aspectRatio,
                0.1f,
                100.0f
        );

        mat4 projectionMatrix =
                perspectiveMatrix * lookAt(glm::vec3(4, 3, 3), vec3(model.modelPos.x, 2.0, model.modelPos.z), vec3(0, 1, 0));

        glLoadMatrixf(glm::value_ptr(projectionMatrix));
    }

    glMatrixMode(GL_MODELVIEW);
    mat4 matrix = scale(mat4(1), vec3(6, 1, 6));
    glLoadMatrixf(glm::value_ptr(matrix));

    road.draw();
    model.draw();

    if (model.wheelRotAngle > 0) {
        model.wheelRotAngle -= 5;
    } else if (model.wheelRotAngle < 0) {
        model.wheelRotAngle += 5;
    }
}

void MainScene::animate() {
    switch (animState) {
        case DOOR_OPENING:
            model.animDoorRot += model.ANIM_SPEED;

            if (model.animDoorRot > model.ANIM_MAX_DOOR_ROT) {
                model.animDoorRot = model.ANIM_MAX_DOOR_ROT;
                animState = DOOR_OPENED;
            }

            break;
        case DOOR_CLOSING:
            model.animDoorRot -= model.ANIM_SPEED;

            if (model.animDoorRot < model.ANIM_MIN_DOOR_ROT) {
                model.animDoorRot = model.ANIM_MIN_DOOR_ROT;
                animState = DOOR_CLOSED;
            }

            break;
    }
}

void MainScene::processKeys() {
    auto isPressed = [this](int key) -> bool { return glfwGetKey(window, key) == GLFW_PRESS; };

    /* Перемещение машины по карте */
    if (isPressed(GLFW_KEY_W)) {
        float step = -model.moveSpeed;
        model.modelPos = vec3(model.modelPos.x + sin(degreesToRadians(model.modelRot)) * step, 0,
                              model.modelPos.z + cos(degreesToRadians(model.modelRot)) * step);
        model.wheelAngle += model.animWheelAngle;//анимации колес
    }

    if (isPressed(GLFW_KEY_S)) {
        float step = +model.moveSpeed;
        model.modelPos = vec3(model.modelPos.x + sin(degreesToRadians(model.modelRot)) * step, 0,
                              model.modelPos.z + cos(degreesToRadians(model.modelRot)) * step);
        model.wheelAngle -= model.animWheelAngle;
    }

    if (isPressed(GLFW_KEY_A)) {
        model.modelRot += model.rotSpeed;
        model.wheelRotAngle = 30;
    }

    if (isPressed(GLFW_KEY_D)) {
        model.modelRot -= model.rotSpeed;
        model.wheelRotAngle = -30;
    }

    /* Изменение вида камеры */
    if (isPressed(GLFW_KEY_1)) {
        viewMode = TOP;
    }

    if (isPressed(GLFW_KEY_2)) {
        viewMode = FIXED_OBSERVE;
    }

    if (isPressed(GLFW_KEY_3)) {
        viewMode = OBSERVE;
    }

    /* Активация анимации */
    if (isPressed(GLFW_KEY_F)) {
        if (animState == DOOR_CLOSED) {
            animState = DOOR_OPENING;
        } else if (animState == DOOR_OPENED) {
            animState = DOOR_CLOSING;
        }
    }
}
