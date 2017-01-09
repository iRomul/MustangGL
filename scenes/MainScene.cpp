#include "MainScene.h"

#include "math_util.h"
#include "SceneLight.h"
#include "SceneFog.h"
#include "SceneColors.h"

#include <glm/ext.hpp>

using namespace std;
using namespace glm;

ViewMode viewMode = OBSERVE;

MainScene::MainScene(GLFWwindow *window) {
    this->window = window;

    SceneLight light1(GL_LIGHT1, SC_PINK, SC_DARK_BLUE, vec4(0.0f, 4.0f, 0.0f, 1.0f));
    light1.enable();

    SceneLight light2(GL_LIGHT2, SC_WHITE, SC_WHITE, vec4(4.0f, -4.0f, 0.0f, 1.0f));
    light2.enable();

    vec3 cameraPos = vec3(6, 6, 6);
    vec3 cameraTarget = vec3(0, 0, 0);

    SceneFog fog(SC_LIGHT_GREY);
    fog.enable();

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
                                lookAt(vec3(model.position.x, 20.0f, model.position.z - 2), model.position,
                                       vec3(0, 0, 1));
        glLoadMatrixf(glm::value_ptr(projectionMatrix));
    } else if (viewMode == FIXED_OBSERVE) {
        //берем матрицу модели
        mat4 modelMatrix = mat4(1);
        modelMatrix = translate(modelMatrix, model.position);
        modelMatrix = rotate(modelMatrix, degreesToRadians(model.rotation), vec3(0, 1, 0));

        mat4 perspectiveMatrix = perspective(
                45.0f,
                aspectRatio,
                0.1f,
                100.0f
        );

        mat4 projectionMatrix =
                perspectiveMatrix *
                lookAt(glm::vec3(4, 3, 3), vec3(model.position.x, 2.0, model.position.z), vec3(0, 1, 0));

        glLoadMatrixf(glm::value_ptr(projectionMatrix));
    }

    glMatrixMode(GL_MODELVIEW);
    mat4 matrix = scale(mat4(1), vec3(6, 1, 6));
    glLoadMatrixf(glm::value_ptr(matrix));

    road.draw();
    model.draw();
}

static double lastTime = 0.0;

void MainScene::animate() {
    double currentTime = glfwGetTime();
    double elapsed = currentTime - lastTime;

    model.animate(elapsed);

    lastTime = currentTime;
}

void MainScene::processKeys() {
    auto isPressed = [this](int key) -> bool { return glfwGetKey(window, key) == GLFW_PRESS; };

    if (isPressed(GLFW_KEY_W)) {
        model.animation.setAccelerationInput(-1.0f);
    }

    if (isPressed(GLFW_KEY_S)) {
        model.animation.setAccelerationInput(1.0f);
    }

    if (isPressed(GLFW_KEY_A)) {
        model.animation.setRotationInput(1.0f);
    }

    if (isPressed(GLFW_KEY_D)) {
        model.animation.setRotationInput(-1.0f);
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
        model.animation.switchDoor();
    }
}
