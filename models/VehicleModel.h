#pragma once

#include <glm/glm.hpp>

#include "texture.hpp"
#include "mesh.hpp"
#include "utilities.h"
#include "AbstractModel.h"

const float ACCELERATION_FACTOR = 10.0f;
const float SPEED_CUTOFF_THRESHOLD = 0.2f;

const float BACKWARD_FRICTION_FACTOR = 0.999f;
const float WHEEL_ROTATION_FACTOR = 2.0f;
const float BACK_WHEEL_ROTATION_FACTOR = 4.5f;

const float DRIVETRAIN_ROTATION_SPEED = degreesToRadians(28); // degree per second
const float DRIVETRAIN_MAX_ROTATION = degreesToRadians(30);
const float DRIVETRAIN_TO_BODY_FACTOR = 200.0f;
const float DRIVETRAIN_TO_BODY_FACTOR_BACKWARD = 100.0f;

const float DOOR_MIN_ANGLE = 0;
const float DOOR_MAX_ANGLE = degreesToRadians(60);
const float DOOR_OPEN_SPEED = degreesToRadians(70);

struct VehicleModelAnimation {
    enum class DoorState {
        DOOR_CLOSED,
        DOOR_OPENING,
        DOOR_OPENED,
        DOOR_CLOSING,
    };

    glm::vec3 velocityVector;

    DoorState doorState = DoorState::DOOR_CLOSED;

    float accelerationInput = 0.0;
    float rotationInput = 0.0;

    VehicleModelAnimation() : velocityVector(0.0f, 0.0f, 0.0f) {
    }

    void setAccelerationInput(float amount) {
        accelerationInput = amount;
    }

    void setRotationInput(float amount) {
        rotationInput = amount;
    }

    void resetInputs() {
        accelerationInput = 0.0f;
        rotationInput = 0.0f;
    }

    void switchDoor() {
        if (doorState == DoorState::DOOR_CLOSED) {
            doorState = DoorState::DOOR_OPENING;
        } else if (doorState == DoorState::DOOR_OPENED) {
            doorState = DoorState::DOOR_CLOSING;
        }
    }
};

class VehicleModel : public AbstractModel {
public:
    Mesh body;
    Mesh wheel;
    Mesh leftDoor;
    Mesh rightDoor;

    VehicleModelAnimation animation;

    glm::vec3 forward = glm::vec3(0, 0, 1);
    glm::vec3 position = glm::vec3(0, 0, 0);
    float rotation = 0.0f;

    float wheelRotation = 0.0f;
    float backWheelRotation = 0.0f;
    float drivetrainRotation = 0.0f;

    float doorRotationStatus = DOOR_MIN_ANGLE;

    VehicleModel();
    void load() override;
    void draw() override;
    void animate(double elapsed);
};