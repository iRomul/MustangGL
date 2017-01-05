#pragma once

#define _USE_MATH_DEFINES
#include "math.h"
#undef _USE_MATH_DEFINES

constexpr float degreesToRadians(const float angle) {
    return angle * static_cast<float>(M_PI) / 180.0f;
}