#pragma once

#include "texture.hpp"
#include "AbstractModel.h"

class RoadModel : AbstractModel {
    Texture groundTexture;

public:
    RoadModel();
    void load() override;
    void draw() override;
};
