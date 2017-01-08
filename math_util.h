#pragma once

#define _USE_MATH_DEFINES
#include "math.h"
#undef _USE_MATH_DEFINES
#include <iostream>

#include <glm/glm.hpp>
#include <glm/detail/precision.hpp>

constexpr float degreesToRadians(const float angle) {
    return angle * static_cast<float>(M_PI) / 180.0f;
}

template <typename T>
T fpnormalize(T val) {
    int val_class = fpclassify(val);
    if (isnormal(val_class)) {
        return val;
    } else {
        return 0;
    }
}

template <typename T, glm::precision P, template <typename, glm::precision> class vecType>
GLM_FUNC_QUALIFIER vecType<T, P> normalize_safe(vecType<T, P> const & x)
{
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'normalize' only accept floating-point inputs");

    auto r = x * glm::inversesqrt(glm::dot(x, x));
    r.x = fpnormalize(r.x);
    r.y = fpnormalize(r.y);
    r.z = fpnormalize(r.z);

    return r;
}