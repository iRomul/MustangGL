#include "VehicleModel.h"

#include <iostream>
#include <fstream>
#include "math_util.h"

#include <glm/ext.hpp>
#include <iomanip>

using namespace std;
using namespace glm;

VehicleModel::VehicleModel() :
        bodyTexture("body.bmp"),
        wheelTexture("wheel.bmp") {


    position = vec3(0, 0, 0);
}

void VehicleModel::load() {
    //load textures
    if (!bodyTexture.load() || !wheelTexture.load()) {
        cerr << "Can't load texture bitmap file" << endl;
        exit(EXIT_FAILURE);
    }

    ifstream modelFile;
    modelFile.exceptions(ios::failbit | ios::badbit);
    modelFile.open("track.mdl", ios::in | ios::binary);

    // TODO: бросать исключения
    if (!rightDoor.load(modelFile, &bodyTexture)) {
        cerr << "Can not load rightDoor mesh" << endl;
//        return false;
    }

    if (!body.load(modelFile, &bodyTexture)) {
        cerr << "Can not load body mesh" << endl;
//        return false;
    }

    if (!wheel.load(modelFile, &wheelTexture)) {
        cerr << "Can not load wheel mesh" << endl;
//        return false;
    }

    if (!leftDoor.load(modelFile, &bodyTexture)) {
        cerr << "Can not load leftDoor mesh" << endl;
//        return false;
    }
}

void VehicleModel::draw() {
//устанавливаем положение модели
    mat4 modelMatrix = mat4(1);
    modelMatrix = translate(modelMatrix, position);
    modelMatrix = rotate(modelMatrix, rotation, vec3(0, 1, 0));
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
    wheelMatrix = rotate(wheelMatrix, 3.14f + drivetrainRotation, vec3(0, 1, 0));
    wheelMatrix = rotate(wheelMatrix, wheelRotation, vec3(1, 0, 0)); //анимация
    //находим матрицу, которая соделжит мировые координаты (из мир. координат модели и относительных колес)
    wheelMatrix = modelMatrix * wheelMatrix;
    glLoadMatrixf(glm::value_ptr(wheelMatrix));
    wheel.draw();

    //заднее левое
    wheelMatrix = mat4(1);
    wheelMatrix = translate(wheelMatrix, vec3(-0.84, 0.45, 1.62));
    wheelMatrix = rotate(wheelMatrix, 3.14f, vec3(0, 1, 0));
    wheelMatrix = rotate(wheelMatrix, wheelRotation, vec3(1, 0, 0));
    wheelMatrix = modelMatrix * wheelMatrix;
    glLoadMatrixf(glm::value_ptr(wheelMatrix));
    wheel.draw();

    //переднее правое
    wheelMatrix = mat4(1);
    wheelMatrix = translate(wheelMatrix, vec3(0.84, 0.45, -1.67));
    wheelMatrix = rotate(wheelMatrix, drivetrainRotation, vec3(0, 1, 0));
    wheelMatrix = rotate(wheelMatrix, -wheelRotation, vec3(1, 0, 0));
    wheelMatrix = modelMatrix * wheelMatrix;
    glLoadMatrixf(glm::value_ptr(wheelMatrix));
    wheel.draw();

    //заднее правое
    wheelMatrix = mat4(1);
    wheelMatrix = translate(wheelMatrix, vec3(0.84, 0.45, 1.62));
    wheelMatrix = rotate(wheelMatrix, -wheelRotation, vec3(1, 0, 0));
    wheelMatrix = modelMatrix * wheelMatrix;
    glLoadMatrixf(glm::value_ptr(wheelMatrix));
    wheel.draw();
}

void VehicleModel::animate(double elapsed) {
    float rotationStep = DRIVETRAIN_ROTATION_SPEED * static_cast<float>(elapsed);
    if (animation.rotationInput != 0.0) {
        if (animation.rotationInput > 0.0) {
            if (drivetrainRotation < DRIVETRAIN_MAX_ROTATION) {
                drivetrainRotation += rotationStep;
            }
        } else {
            if (drivetrainRotation > -DRIVETRAIN_MAX_ROTATION) {
                drivetrainRotation -= rotationStep;
            }
        }
    } else {
        if (!fuzzEqual(static_cast<double>(drivetrainRotation), 0.0, 1E-4)) {
            if (drivetrainRotation > 0.0) {
                drivetrainRotation -= rotationStep;
            } else if (drivetrainRotation < 0.0) {
                drivetrainRotation += rotationStep;
            }
        }
    }

    // FIXME: Исправить реальное направление вектора forward
    int directionSign = angle(forward, normalize_safe(animation.velocityVector)) >= degreesToRadians(90) ? 1 : -1;

    float relativeRotation = drivetrainRotation / DRIVETRAIN_TO_BODY_FACTOR;
    relativeRotation *= directionSign * length(animation.velocityVector) / 10;

    forward = rotateY(forward, relativeRotation);

    vec3 acceleration = forward * animation.accelerationInput * ACCELERATION_FACTOR;

    vec3 backwardFriction = -animation.velocityVector * BACKWARD_FRICTION_FACTOR;
    animation.velocityVector += (backwardFriction) * elapsed;

    float currentSpeed = length(animation.velocityVector);
    if (currentSpeed < SPEED_CUTOFF_THRESHOLD && animation.accelerationInput == 0.0) {
        animation.velocityVector = vec3(0.0f, 0.0f, 0.0f);
    } else {
        animation.velocityVector += acceleration * elapsed;
    }

    rotation += relativeRotation;
    position += animation.velocityVector * elapsed;
    wheelRotation += directionSign * currentSpeed * WHEEL_ROTATION_FACTOR * elapsed;

    using DoorState = VehicleModelAnimation::DoorState;
    switch (animation.doorState) {
        case DoorState::DOOR_OPENING: {
            animDoorRot += ANIM_SPEED;

            if (animDoorRot > ANIM_MAX_DOOR_ROT) {
                animDoorRot = ANIM_MAX_DOOR_ROT;
                animation.doorState = DoorState::DOOR_OPENED;
            }

            break;
        }

        case DoorState::DOOR_CLOSING: {
            animDoorRot -= ANIM_SPEED;

            if (animDoorRot < ANIM_MIN_DOOR_ROT) {
                animDoorRot = ANIM_MIN_DOOR_ROT;
                animation.doorState = DoorState::DOOR_CLOSED;
            }

            break;
        }

        default: {
            break;
        }
    }

    animation.resetInputs();
}
