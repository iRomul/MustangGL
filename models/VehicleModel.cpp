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
    ifstream wheelModel("ms_wheel.obj");
    ifstream bodyModel("ms_body.obj");
    ifstream leftDoorModel("ms_ldoor.obj");
    ifstream rightDoorModel("ms_rdoor.obj");

    if (!wheelModel || !bodyModel || !leftDoorModel || !rightDoorModel) {
        cerr << "Can't open model file" << endl;
        throw runtime_error("Model file is unavailable");
    }

    if (!rightDoor.loadObj(rightDoorModel)) {
        cerr << "Can not load rightDoor mesh" << endl;
        throw runtime_error("Right door model parse failed");
    }

    if (!body.loadObj(bodyModel)) {
        cerr << "Can not load body mesh" << endl;
        throw runtime_error("Body model parse failed");
    }

    if (!wheel.loadObj(wheelModel)) {
        cerr << "Can not load wheel mesh" << endl;
        throw runtime_error("Wheel model parse failed");
    }

    if (!leftDoor.loadObj(leftDoorModel)) {
        cerr << "Can not load leftDoor mesh" << endl;
        throw runtime_error("Left door model parse failed");
    }
}

void VehicleModel::draw() {
    /* Drawing body */
    glm::vec4 ambient(0.1745, 0.01175, 0.01175, 0.55);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glm::value_ptr(ambient));
    glm::vec4 diffuse(0.61424, 0.04136, 0.04136, 0.55);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm::value_ptr(diffuse));
    glm::vec4 specular(0.727811, 0.626959, 0.626959, 0.55);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr(specular));
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 76.8f);
    // Loading identity matrix
    mat4 modelMatrix = mat4(1);
    // Moving model from word center
    modelMatrix = translate(modelMatrix, position + vec3(0, 2, 0));
    // Rotating model from pivot
    modelMatrix = rotate(modelMatrix, rotation, vec3(0, 1, 0));
    // Loading matrix
    glLoadMatrixf(glm::value_ptr(modelMatrix));
    // Drawing model
    body.draw();

    const float DOOR_Y_OFFSET = -0.22f;
    const float DOOR_Z_OFFSET = -0.88f;

    /* Drawing doors */
    // Left door
    mat4 leftDoorMatrix = mat4(1);
    leftDoorMatrix = translate(leftDoorMatrix, vec3(-1.16, DOOR_Y_OFFSET, DOOR_Z_OFFSET));
    leftDoorMatrix = rotate(leftDoorMatrix, -doorRotationStatus, vec3(0, 1, 0));
    leftDoorMatrix = modelMatrix * leftDoorMatrix;
    glLoadMatrixf(glm::value_ptr(leftDoorMatrix));
    leftDoor.draw();

    // Right door
    mat4 rightDoorMatrix = mat4(1);
    rightDoorMatrix = translate(rightDoorMatrix, vec3(1.175, DOOR_Y_OFFSET, DOOR_Z_OFFSET));
    rightDoorMatrix = rotate(rightDoorMatrix, doorRotationStatus, vec3(0, 1, 0));
    rightDoorMatrix = modelMatrix * rightDoorMatrix;
    glLoadMatrixf(glm::value_ptr(rightDoorMatrix));
    rightDoor.draw();

    const float WHEEL_Y_OFFSET = -0.6f;
    const float WHEEL_FRONT_Z_OFFSET = -2.15f;
    const float WHEEL_REAR_Z_OFFSET = 1.75f;
    const float WHEEL_X_OFFSET = 1.07f;

    /* Drawing wheels */
    glm::vec4 rubberAmbient(0.02, 0.02, 0.02, 1.0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glm::value_ptr(rubberAmbient));
    glm::vec4 rubberDiffuse(0.4, 0.4, 0.4, 1.0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm::value_ptr(rubberDiffuse));
    glm::vec4 rubberSpecular(0.4, 0.4, 0.4, 1.0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr(rubberSpecular));
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0f);
    // Front left wheel
    mat4 wheelMatrix = mat4(1);
    wheelMatrix = translate(wheelMatrix, vec3(-WHEEL_X_OFFSET, WHEEL_Y_OFFSET, WHEEL_FRONT_Z_OFFSET));
    wheelMatrix = rotate(wheelMatrix, 3.14f + drivetrainRotation, vec3(0, 1, 0));
    wheelMatrix = rotate(wheelMatrix, wheelRotation, vec3(1, 0, 0));
    wheelMatrix = modelMatrix * wheelMatrix;
    glLoadMatrixf(glm::value_ptr(wheelMatrix));
    wheel.draw();

    // Rare left
    wheelMatrix = mat4(1);
    wheelMatrix = translate(wheelMatrix, vec3(-WHEEL_X_OFFSET, WHEEL_Y_OFFSET, WHEEL_REAR_Z_OFFSET));
    wheelMatrix = rotate(wheelMatrix, 3.14f, vec3(0, 1, 0));
    wheelMatrix = rotate(wheelMatrix, backWheelRotation, vec3(1, 0, 0));
    wheelMatrix = modelMatrix * wheelMatrix;
    glLoadMatrixf(glm::value_ptr(wheelMatrix));
    wheel.draw();

    // Front right
    wheelMatrix = mat4(1);
    wheelMatrix = translate(wheelMatrix, vec3(WHEEL_X_OFFSET, WHEEL_Y_OFFSET, WHEEL_FRONT_Z_OFFSET));
    wheelMatrix = rotate(wheelMatrix, drivetrainRotation, vec3(0, 1, 0));
    wheelMatrix = rotate(wheelMatrix, -wheelRotation, vec3(1, 0, 0));
    wheelMatrix = modelMatrix * wheelMatrix;
    glLoadMatrixf(glm::value_ptr(wheelMatrix));
    wheel.draw();

    // Rare right
    wheelMatrix = mat4(1);
    wheelMatrix = translate(wheelMatrix, vec3(WHEEL_X_OFFSET, WHEEL_Y_OFFSET, WHEEL_REAR_Z_OFFSET));
    wheelMatrix = rotate(wheelMatrix, -backWheelRotation, vec3(1, 0, 0));
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

    int directionSign = angle(forward, normalize_safe(animation.velocityVector)) >= degreesToRadians(90) ? 1 : -1;

    float deviationAngle = length(cross(directionSign * forward, normalize(animation.velocityVector)));
    deviationAngle = fpnormalize(deviationAngle);


    float drivetrainToBodyFactor;

    if (directionSign == -1) {
        drivetrainToBodyFactor = DRIVETRAIN_TO_BODY_FACTOR_BACKWARD;
    } else {
        drivetrainToBodyFactor = DRIVETRAIN_TO_BODY_FACTOR;
    }

    float relativeRotation = drivetrainRotation / drivetrainToBodyFactor;
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

    float backWheelFactor;

    if (animation.accelerationInput < 0.0) {
        backWheelFactor = BACK_WHEEL_ROTATION_FACTOR;
    } else {
        backWheelFactor = WHEEL_ROTATION_FACTOR;
    }

    backWheelRotation += directionSign * currentSpeed * backWheelFactor * elapsed;

    using DoorState = VehicleModelAnimation::DoorState;
    switch (animation.doorState) {
        case DoorState::DOOR_OPENING: {
            doorRotationStatus += DOOR_OPEN_SPEED * elapsed;

            if (doorRotationStatus > DOOR_MAX_ANGLE) {
                doorRotationStatus = DOOR_MAX_ANGLE;
                animation.doorState = DoorState::DOOR_OPENED;
            }

            break;
        }

        case DoorState::DOOR_CLOSING: {
            doorRotationStatus -= DOOR_OPEN_SPEED * elapsed;

            if (doorRotationStatus < DOOR_MIN_ANGLE) {
                doorRotationStatus = DOOR_MIN_ANGLE;
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
