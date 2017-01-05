#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "texture.hpp"
#include "mesh.hpp"
#include "math_util.h"

using namespace std;
using namespace glm;
using namespace glm::gtc;

vec3 cameraPos = vec3(6, 6, 6);
vec3 cameraTarget = vec3(0, 0, 0);

vec3 modelPos = vec3(-3, 0, 0);
float modelRot = 0;
float wheelAngle = 0;
i32 wheelRotAngle = 0;

const float animWheelAngle = 0.1;
const float moveSpeed = 0.1;
const float rotSpeed = 2;

vec4 lightAmbient = vec4(0.8f, 0.8f, 0.8f, 1.0f);  // Значения фонового свет
vec4 lightDiffuse = vec4(0.4f, 0.6f, 1.0f, 1.0f);  // Значения диффузного света
vec4 lightPosition = vec4(0.0f, 4.0f, 0.0f, 1.0f); // Позиция света (w = 1)

Texture bodyTexture("body.bmp");
Texture wheelTexture("wheel.bmp");
Texture groundTexture("ground.bmp");

Mesh body;
Mesh wheel;
Mesh leftDoor;
Mesh rightDoor;

enum ViewMode {
    TOP,//вид сверху
    OBSERVE,//вид откуда-то с башни
    FIXED_OBSERVE,//вид как в РПГшке
};

ViewMode viewMode = OBSERVE;

enum AnimationState {
    DOOR_CLOSED,
    DOOR_OPENING,
    DOOR_OPENED,
    DOOR_CLOSING,
};

AnimationState animState = DOOR_CLOSED;

const float ANIM_MIN_DOOR_ROT = 0;
const float ANIM_MAX_DOOR_ROT = 60;
const float ANIM_SPEED = 2;

float animDoorRot = ANIM_MIN_DOOR_ROT;

//функция загружает модель из файла. Конвертер из COLLADA просто выплевывает меши в файл модели, а мы их загружаем
bool loadModel() {
    FILE *file = fopen("track.mdl", "rb");

    if (!file) {
        cout << "can not read model file" << endl;
        return false;
    }

    if (!rightDoor.load(file, &bodyTexture)) {
        cout << "Can not load rightDoor mesh" << endl;
        return false;
    }

    if (!body.load(file, &bodyTexture)) {
        cout << "Can not load body mesh" << endl;
        return false;
    }

    if (!wheel.load(file, &wheelTexture)) {
        cout << "Can not load wheel mesh" << endl;
        return false;
    }

    if (!leftDoor.load(file, &bodyTexture)) {
        cout << "Can not load leftDoor mesh" << endl;
        return false;
    }

    fclose(file);

    return true;
}

void drawModel() {
    //устанавливаем положение модели
    mat4 modelMatrix = mat4(1);
    modelMatrix = translate(modelMatrix, modelPos);
    modelMatrix = rotate(modelMatrix, degreesToRadians(modelRot), vec3(0, 1, 0));
    //отправляем матрицу OpenGL
    glLoadMatrixf(glm::value_ptr(modelMatrix));
    //рисуем корпус
    body.draw();

    //устанавливаем положение дверей
    //левая
    mat4 leftDoorMatrix = mat4(1);
    leftDoorMatrix = translate(leftDoorMatrix, vec3(-1.16, 1.05, -1.55));
    leftDoorMatrix = rotate(leftDoorMatrix, degreesToRadians(-animDoorRot), vec3(0, 1, 0));
    //находим матрицу, которая соделжит мировые координаты(из мир. координат модели и относительных колес)
    leftDoorMatrix = modelMatrix * leftDoorMatrix;
    glLoadMatrixf(glm::value_ptr(leftDoorMatrix));
    leftDoor.draw();

    //устанавливаем положение дверей
    //правая
    mat4 rightDoorMatrix = mat4(1);
    rightDoorMatrix = translate(rightDoorMatrix, vec3(1.175, 1.05, -1.635));
    rightDoorMatrix = rotate(rightDoorMatrix, degreesToRadians(animDoorRot), vec3(0, 1, 0));
    //находим матрицу, которая соделжит мировые координаты (из мир. координат модели и относительных колес)
    rightDoorMatrix = modelMatrix * rightDoorMatrix;
    glLoadMatrixf(glm::value_ptr(rightDoorMatrix));
    rightDoor.draw();

    //устанавливаем положение колес
    //переднее левое
    mat4 wheelMatrix = mat4(1);
    wheelMatrix = translate(wheelMatrix, vec3(-0.84, 0.45, -1.67));
    wheelMatrix = rotate(wheelMatrix, 3.14f + degreesToRadians(wheelRotAngle), vec3(0, 1, 0));
    wheelMatrix = rotate(wheelMatrix, wheelAngle, vec3(1, 0, 0)); //анимация
    //находим матрицу, которая соделжит мировые координаты (из мир. координат модели и относительных колес)
    wheelMatrix = modelMatrix * wheelMatrix;
    glLoadMatrixf(glm::value_ptr(wheelMatrix));
    wheel.draw();

    //заднее левое
    wheelMatrix = mat4(1);
    wheelMatrix = translate(wheelMatrix, vec3(-0.84, 0.45, 1.62));
    wheelMatrix = rotate(wheelMatrix, 3.14f, vec3(0, 1, 0));
    wheelMatrix = rotate(wheelMatrix, wheelAngle, vec3(1, 0, 0));
    wheelMatrix = modelMatrix * wheelMatrix;
    glLoadMatrixf(glm::value_ptr(wheelMatrix));
    wheel.draw();

    //переднее правое
    wheelMatrix = mat4(1);
    wheelMatrix = translate(wheelMatrix, vec3(0.84, 0.45, -1.67));
    wheelMatrix = rotate(wheelMatrix, degreesToRadians(wheelRotAngle), vec3(0, 1, 0));
    wheelMatrix = rotate(wheelMatrix, -wheelAngle, vec3(1, 0, 0));
    wheelMatrix = modelMatrix * wheelMatrix;
    glLoadMatrixf(glm::value_ptr(wheelMatrix));
    wheel.draw();

    //заднее правое
    wheelMatrix = mat4(1);
    wheelMatrix = translate(wheelMatrix, vec3(0.84, 0.45, 1.62));
    wheelMatrix = rotate(wheelMatrix, -wheelAngle, vec3(1, 0, 0));
    wheelMatrix = modelMatrix * wheelMatrix;
    glLoadMatrixf(glm::value_ptr(wheelMatrix));
    wheel.draw();
}

//рисуем пол
void drawFloor() {
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

GLFWwindow *window;

void processKeys() {
    auto isPressed = [](int key) -> bool { return glfwGetKey(window, key) == GLFW_PRESS; };

    /* Перемещение машины по карте */
    if (isPressed(GLFW_KEY_W)) {
        float step = -moveSpeed;
        modelPos = vec3(modelPos.x + sin(degreesToRadians(modelRot)) * step, 0,
                        modelPos.z + cos(degreesToRadians(modelRot)) * step);
        wheelAngle += animWheelAngle;//анимации колес
    }

    if (isPressed(GLFW_KEY_S)) {
        float step = +moveSpeed;
        modelPos = vec3(modelPos.x + sin(degreesToRadians(modelRot)) * step, 0,
                        modelPos.z + cos(degreesToRadians(modelRot)) * step);
        wheelAngle -= animWheelAngle;
    }

    if (isPressed(GLFW_KEY_A)) {
        modelRot += rotSpeed;
        wheelRotAngle = 30;
    }

    if (isPressed(GLFW_KEY_D)) {
        modelRot -= rotSpeed;
        wheelRotAngle = -30;
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

void animate() {
    switch (animState) {
        case DOOR_OPENING:
            animDoorRot += ANIM_SPEED;

            if (animDoorRot > ANIM_MAX_DOOR_ROT) {
                animDoorRot = ANIM_MAX_DOOR_ROT;
                animState = DOOR_OPENED;
            }

            break;
        case DOOR_CLOSING:
            animDoorRot -= ANIM_SPEED;

            if (animDoorRot < ANIM_MIN_DOOR_ROT) {
                animDoorRot = ANIM_MIN_DOOR_ROT;
                animState = DOOR_CLOSED;
            }

            break;
    }
}

void drawScene() {
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
                                lookAt(vec3(modelPos.x, 20.0f, modelPos.z - 2), modelPos, vec3(0, 0, 1));
        glLoadMatrixf(glm::value_ptr(projectionMatrix));
    } else if (viewMode == FIXED_OBSERVE) {
        //берем матрицу модели
        mat4 modelMatrix = mat4(1);
        modelMatrix = translate(modelMatrix, modelPos);
        modelMatrix = rotate(modelMatrix, degreesToRadians(modelRot), vec3(0, 1, 0));

        //устанавливаем камеру относительно модели и получим ее мировые координаты
        //vec3 cameraPos = modelMatrix * vec4(0.0, 5.0, 8.0, 1.0); //w будет 1

        mat4 perspectiveMatrix = perspective(
                45.0f,
                aspectRatio,
                0.1f,
                100.0f
        );

        mat4 projectionMatrix =
                perspectiveMatrix * lookAt(glm::vec3(4, 3, 3), vec3(modelPos.x, 2.0, modelPos.z), vec3(0, 1, 0));

        glLoadMatrixf(glm::value_ptr(projectionMatrix));
    }

    glMatrixMode(GL_MODELVIEW);
    mat4 matrix = scale(mat4(1), vec3(6, 1, 6));
    glLoadMatrixf(glm::value_ptr(matrix));
    drawFloor();

    drawModel();

    if (wheelRotAngle > 0) {
        wheelRotAngle -= 5;
    } else if (wheelRotAngle < 0) {
        wheelRotAngle += 5;
    }
}

int main() {
    std::cout.sync_with_stdio(false);

    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    window = glfwCreateWindow(1024, 768, "Tutorial 0 - Keyboard and Mouse", nullptr, nullptr);
    if (window == nullptr) {
        cerr << "Failed to open GLFW window." << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW" << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // #b1deff - blue sky color
    glClearColor(0.694f, 0.871f, 1.0f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glLightfv(GL_LIGHT1, GL_AMBIENT, &lightAmbient[0]);        // установим Ambient Light
    glLightfv(GL_LIGHT1, GL_DIFFUSE, &lightDiffuse[0]);        // установим Diffuse Light
    glLightfv(GL_LIGHT1, GL_POSITION, glm::value_ptr(lightPosition));    // установим позицию The Light
    glEnable(GL_LIGHT1);                                // включим

    //load textures
    if (!bodyTexture.load() || !wheelTexture.load() || !groundTexture.load()) {
        cerr << "Can't load texture bitmap file" << endl;
        exit(EXIT_FAILURE);
    }

    //load model
    if (!loadModel()) {
        cerr << "Can't load model file" << endl;
        exit(EXIT_FAILURE);
    }

    do {
        processKeys();

        animate();

        drawScene();

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}
