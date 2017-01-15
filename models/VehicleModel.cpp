#include "VehicleModel.h"

#include <iostream>
#include <fstream>
#include "utilities.h"

#include <glm/ext.hpp>
#include <iomanip>

using namespace std;
using namespace glm;

VehicleModel::VehicleModel() {
    position = vec3(0, 0, 0);
}

void VehicleModel::load() {
    //load textures

    ifstream modelFile;
    modelFile.exceptions(ios::failbit | ios::badbit);
    modelFile.open("track.mdl", ios::in | ios::binary);

    ifstream wheelModel("ms_wheel.obj");
    ifstream bodyModel("ms_body.obj");

    if (!wheelModel || !bodyModel) {
        cerr << "Can't open model file" << endl;
        throw runtime_error("Model file is unavailable");
    }

    if (!rightDoor.load(modelFile)) {
        cerr << "Can not load rightDoor mesh" << endl;
    }

    if (!body.loadObj(bodyModel)) {
        cerr << "Can not load body mesh" << endl;
        throw runtime_error("Body model parse failed");
    }

    if (!wheel.loadObj(wheelModel)) {
        cerr << "Can not load wheel mesh" << endl;
        throw runtime_error("Wheel model parse failed");
    }
//
//    if (!leftDoor.load(modelFile)) {
//        cerr << "Can not load leftDoor mesh" << endl;
////        return false;
//    }
}

void VehicleModel::draw() {
    glDisable(GL_TEXTURE_2D);

    mat4 modelMatrix = mat4(1);
    modelMatrix = translate(modelMatrix, position + vec3(0, 2, 0));
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

    const float WHEEL_Y_OFFSET = -0.6f;
    const float WHEEL_FRONT_Z_OFFSET = -2.15f;
    const float WHEEL_REAR_Z_OFFSET = 1.75f;
    const float WHEEL_X_OFFSET = 1.07f;

    //устанавливаем положение колес
    //переднее левое
    mat4 wheelMatrix = mat4(1);
    wheelMatrix = translate(wheelMatrix, vec3(-WHEEL_X_OFFSET, WHEEL_Y_OFFSET, WHEEL_FRONT_Z_OFFSET));
    wheelMatrix = rotate(wheelMatrix, 3.14f + drivetrainRotation, vec3(0, 1, 0));
    wheelMatrix = rotate(wheelMatrix, wheelRotation, vec3(1, 0, 0)); //анимация
    //находим матрицу, которая соделжит мировые координаты (из мир. координат модели и относительных колес)
    wheelMatrix = modelMatrix * wheelMatrix;
    glLoadMatrixf(glm::value_ptr(wheelMatrix));
    wheel.draw();

    //заднее левое
    wheelMatrix = mat4(1);
    wheelMatrix = translate(wheelMatrix, vec3(-WHEEL_X_OFFSET, WHEEL_Y_OFFSET, WHEEL_REAR_Z_OFFSET));
    wheelMatrix = rotate(wheelMatrix, 3.14f, vec3(0, 1, 0));
    wheelMatrix = rotate(wheelMatrix, wheelRotation, vec3(1, 0, 0));
    wheelMatrix = modelMatrix * wheelMatrix;
    glLoadMatrixf(glm::value_ptr(wheelMatrix));
    wheel.draw();

    //переднее правое
    wheelMatrix = mat4(1);
    wheelMatrix = translate(wheelMatrix, vec3(WHEEL_X_OFFSET, WHEEL_Y_OFFSET, WHEEL_FRONT_Z_OFFSET));
    wheelMatrix = rotate(wheelMatrix, drivetrainRotation, vec3(0, 1, 0));
    wheelMatrix = rotate(wheelMatrix, -wheelRotation, vec3(1, 0, 0));
    wheelMatrix = modelMatrix * wheelMatrix;
    glLoadMatrixf(glm::value_ptr(wheelMatrix));
    wheel.draw();

    //заднее правое
    wheelMatrix = mat4(1);
    wheelMatrix = translate(wheelMatrix, vec3(WHEEL_X_OFFSET, WHEEL_Y_OFFSET, WHEEL_REAR_Z_OFFSET));
    wheelMatrix = rotate(wheelMatrix, -wheelRotation, vec3(1, 0, 0));
    wheelMatrix = modelMatrix * wheelMatrix;
    glLoadMatrixf(glm::value_ptr(wheelMatrix));
    wheel.draw();

    glEnable(GL_TEXTURE_2D);
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

    float deviationAngle = length(cross(directionSign * forward, normalize(animation.velocityVector)));
    deviationAngle = fpnormalize(deviationAngle);

    float relativeRotation = drivetrainRotation / DRIVETRAIN_TO_BODY_FACTOR;
    relativeRotation *= directionSign * length(animation.velocityVector) / 15;

    forward = rotateY(forward, relativeRotation);

    vec3 acceleration = forward * animation.accelerationInput * ACCELERATION_FACTOR;

    vec3 backwardFriction = -animation.velocityVector * BACKWARD_FRICTION_FACTOR;
    animation.velocityVector += (backwardFriction) * elapsed;
    animation.velocityVector *= 1 - pow(deviationAngle, 13.0f) / 1.5f;

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
