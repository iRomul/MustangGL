#pragma once

#include <texture.hpp>
#include <mesh.hpp>
#include <glm/glm.hpp>
#include "AbstractModel.h"

class VehicleModel : public AbstractModel {
public:
    Texture bodyTexture;
    Texture wheelTexture;

    Mesh body;
    Mesh wheel;
    Mesh leftDoor;
    Mesh rightDoor;

    glm::vec3 modelPos = glm::vec3(0, 0, 0);
    glm::vec3 forward = glm::vec3(0, 0, 1);
    float modelRot = 0;
    float wheelAngle = 0;
    int32_t wheelRotAngle = 0;

    const float rotSpeed = 0.5;

    const float ANIM_MIN_DOOR_ROT = 0;
    const float ANIM_MAX_DOOR_ROT = 60;
    const float ANIM_SPEED = 2;

    float animDoorRot = ANIM_MIN_DOOR_ROT;

    VehicleModel();
    void load() override;
    void draw() override;
};