#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "texture.hpp"
#include "mesh.hpp"
#include "window.hpp"

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

GLfloat lightAmbient[] = {0.8f, 0.8f, 0.8f, 1.0f}; // Значения фонового свет
GLfloat lightDiffuse[] = {0.4f, 0.6f, 1.0f, 1.0f}; // Значения диффузного света
vec4 lightPosition = vec4(0.0f, 4.0f, 0.0f, 1.0f);// Позиция света (w=1)

Texture tex1;
Texture tex2;
Texture tex3;

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
    Closed,
    Opening,
    Opened,
    Closing,
};

AnimationState animState = Closed;

const float ANIM_MIN_DOOR_ROT = 0;
const float ANIM_MAX_DOOR_ROT = 60;
const float ANIM_SPEED = 2;

float animDoorRot = ANIM_MIN_DOOR_ROT;

float toRad(float d) {
    return d / 57.5f;
}

//функция загружает модель из файла. Конвертер из COLLADA просто выплевывает меши в файл модели, а мы их загружаем
bool loadModel() {
    FILE *file = fopen("track.mdl", "rb");
    if (!file) {
        printf("can not read model file\n");
        return false;
    }

    if (!rightDoor.load(file, &tex1)) {
        printf("Can not load rightDoor mesh");
        return false;
    }

    if (!body.load(file, &tex1)) {
        printf("Can not load body mesh");
        return false;
    }

    if (!wheel.load(file, &tex2)) {
        printf("Can not load wheel mesh");
        return false;
    }

    if (!leftDoor.load(file, &tex1)) {
        printf("Can not load leftDoor mesh");
        return false;
    }

    fclose(file);

    return true;
}

void drawModel() {
    //устанавливаем положение модели
    mat4 modelMatrix = mat4(1);
    modelMatrix = translate(modelMatrix, modelPos);
    modelMatrix = rotate(modelMatrix, toRad(modelRot), vec3(0, 1, 0));
    glLoadMatrixf((float *) glm::value_ptr(modelMatrix));//отправляем матрицу OpenGL
    body.draw();//рисуем корпус

    //устанавливаем положение дверей
    //левая
    mat4 leftDoorMatrix = mat4(1);
    leftDoorMatrix = translate(leftDoorMatrix, vec3(-1.16, 1.05, -1.55));
    leftDoorMatrix = rotate(leftDoorMatrix, toRad(-animDoorRot), vec3(0, 1, 0));
    leftDoorMatrix = modelMatrix *
                     leftDoorMatrix;//находим матрицу, которая соделжит мировые координаты(из мир. координат модели и относительных колес)
    glLoadMatrixf((float *) glm::value_ptr(leftDoorMatrix));
    leftDoor.draw();

    //устанавливаем положение дверей
    //правая
    mat4 rightDoorMatrix = mat4(1);
    rightDoorMatrix = translate(rightDoorMatrix, vec3(1.175, 1.05, -1.635));
    rightDoorMatrix = rotate(rightDoorMatrix, toRad(animDoorRot), vec3(0, 1, 0));
    rightDoorMatrix = modelMatrix *
                      rightDoorMatrix;//находим матрицу, которая соделжит мировые координаты(из мир. координат модели и относительных колес)
    glLoadMatrixf((float *) glm::value_ptr(rightDoorMatrix));
    rightDoor.draw();

    //устанавливаем положение колес
    //переднее левое
    mat4 wheelMatrix = mat4(1);
    wheelMatrix = translate(wheelMatrix, vec3(-0.84, 0.45, -1.67));
    wheelMatrix = rotate(wheelMatrix, 3.14f + toRad(wheelRotAngle), vec3(0, 1, 0));
    wheelMatrix = rotate(wheelMatrix, wheelAngle, vec3(1, 0, 0));//анимация
    wheelMatrix = modelMatrix *
                  wheelMatrix;//находим матрицу, которая соделжит мировые координаты(из мир. координат модели и относительных колес)
    glLoadMatrixf((float *) glm::value_ptr(wheelMatrix));
    wheel.draw();


    //заднее левое
    wheelMatrix = mat4(1);
    wheelMatrix = translate(wheelMatrix, vec3(-0.84, 0.45, 1.62));
    wheelMatrix = rotate(wheelMatrix, 3.14f, vec3(0, 1, 0));
    wheelMatrix = rotate(wheelMatrix, wheelAngle, vec3(1, 0, 0));
    wheelMatrix = modelMatrix * wheelMatrix;
    glLoadMatrixf((float *) glm::value_ptr(wheelMatrix));
    wheel.draw();

    //переднее правое
    wheelMatrix = mat4(1);
    wheelMatrix = translate(wheelMatrix, vec3(0.84, 0.45, -1.67));
    wheelMatrix = rotate(wheelMatrix, toRad(wheelRotAngle), vec3(0, 1, 0));
    wheelMatrix = rotate(wheelMatrix, -wheelAngle, vec3(1, 0, 0));
    wheelMatrix = modelMatrix * wheelMatrix;
    glLoadMatrixf((float *) glm::value_ptr(wheelMatrix));
    wheel.draw();

    //заднее правое
    wheelMatrix = mat4(1);
    wheelMatrix = translate(wheelMatrix, vec3(0.84, 0.45, 1.62));
    wheelMatrix = rotate(wheelMatrix, -wheelAngle, vec3(1, 0, 0));
    wheelMatrix = modelMatrix * wheelMatrix;
    glLoadMatrixf((float *) glm::value_ptr(wheelMatrix));
    wheel.draw();
}

//рисуем пол
void drawFloor() {
    tex3.bind();

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

void processKeys() {
    if (keys[0x57]) {//w
        float step = -moveSpeed;
        modelPos = vec3(modelPos.x + sin(toRad(modelRot)) * step, 0, modelPos.z + cos(toRad(modelRot)) * step);
        wheelAngle += animWheelAngle;//анимации колес
    }

    if (keys[0x53]) {//s
        float step = +moveSpeed;
        modelPos = vec3(modelPos.x + sin(toRad(modelRot)) * step, 0, modelPos.z + cos(toRad(modelRot)) * step);
        wheelAngle -= animWheelAngle;
    }

    if (keys[0x41]) {//a
        modelRot += rotSpeed;
        wheelRotAngle = 30;
    }

    if (keys[0x44]) {
        modelRot -= rotSpeed;
        wheelRotAngle = -30;
    }

    if (keys[0x31]) {
        viewMode = TOP;
    }

    if (keys[0x32]) {
        viewMode = FIXED_OBSERVE;
    }

    if (keys[0x33]) {
        viewMode = OBSERVE;
    }

    if (keys[0x20]) {//space
        if (animState == Closed) {
            animState = Opening;
        } else if (animState == Opened) {
            animState = Closing;
        }
    }
}

void animate() {
    switch (animState) {
        case Opening:
            animDoorRot += ANIM_SPEED;

            if (animDoorRot > ANIM_MAX_DOOR_ROT) {
                animDoorRot = ANIM_MAX_DOOR_ROT;
                animState = Opened;
            }

            break;
        case Closing:
            animDoorRot -= ANIM_SPEED;

            if (animDoorRot < ANIM_MIN_DOOR_ROT) {
                animDoorRot = ANIM_MIN_DOOR_ROT;
                animState = Closed;
            }

            break;
    }
}

void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    if (viewMode == OBSERVE) {
        mat4 projectionMatrix = perspective(45.0f, (GLfloat) windowWidth / (GLfloat) windowHeight, 0.1f, 100.0f) *
                                lookAt(cameraPos, cameraTarget, vec3(0, 1, 0));
        glLoadMatrixf((float *) glm::value_ptr(projectionMatrix));
    } else if (viewMode == TOP) {
        mat4 projectionMatrix = perspective(45.0f, (GLfloat) windowWidth / (GLfloat) windowHeight, 0.1f, 100.0f) *
                                lookAt(vec3(modelPos.x, 20.0f, modelPos.z - 2), modelPos, vec3(0, 0, 1));
        glLoadMatrixf((float *) glm::value_ptr(projectionMatrix));
    } else if (viewMode == FIXED_OBSERVE) {
        //берем матрицу модели
        mat4 modelMatrix = mat4(1);
        modelMatrix = translate(modelMatrix, modelPos);
        modelMatrix = rotate(modelMatrix, toRad(modelRot), vec3(0, 1, 0));

        //устанавливаем камеру относительно модели и получим ее мировые координаты
        //vec3 cameraPos = modelMatrix * vec4(0.0, 5.0, 8.0, 1.0); //w будет 1

        mat4 persectiveMatrix = perspective(
                45.0f,
                (GLfloat) windowWidth / (GLfloat) windowHeight,
                0.1f,
                100.0f
        );

        mat4 projectionMatrix = persectiveMatrix * lookAt(glm::vec3(4,3,3), vec3(modelPos.x, 2.0, modelPos.z), vec3(0, 1, 0));

        glLoadMatrixf((float *) glm::value_ptr(projectionMatrix));
    }

    glMatrixMode(GL_MODELVIEW);
    mat4 matrix = scale(mat4(1), vec3(6, 1, 6));
    glLoadMatrixf((float *) glm::value_ptr(matrix));
    drawFloor();

    drawModel();

    if (wheelRotAngle > 0) {
        wheelRotAngle -= 5;
    } else if (wheelRotAngle < 0) {
        wheelRotAngle += 5;
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char *lpCmdLine, int nCmdShow) {
// Create Our OpenGL Window
    if (!createWindow("Rodion", 800, 600, false)) {
        return 0;
    }

    configGL();

    glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);        // установим Ambient Light
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);        // установим Diffuse Light
    glLightfv(GL_LIGHT1, GL_POSITION, (float *) glm::value_ptr(lightPosition));    // установим позицию The Light
    glEnable(GL_LIGHT1);                                // включим

//load textures
    if (!tex1.load("body.bmp")) {
        printf("can not load body.bmp");
        return 1;
    }
    if (!tex2.load("wheel.bmp")) {
        printf("can not load wheel.bmp");
        return 1;
    }
    if (!tex3.load("ground.bmp")) {
        printf("can not load ground.bmp");
        return 1;
    }

//load model
    if (!loadModel()) {
        printf("Can not load model");
        return 1;
    }

    MSG msg;
    bool shouldExit = false;

    while (!shouldExit) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) { //Quit on X button
                shouldExit = true;
            } else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        } else {
            if (active) {
                if (keys[VK_ESCAPE]) { //Quit ion escape
                    shouldExit = true;
                } else {
                    processKeys();

                    animate();

                    drawScene();
                    SwapBuffers(hDC);                // Swap Buffers (Double Buffering)
                }
            }
        }
    }

// Shutdown
    killWindow();                                    // Kill The Window
    return (msg.wParam);                            // Exit The Program

    return 0;
}
