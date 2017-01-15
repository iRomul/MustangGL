#pragma once

#define _USE_MATH_DEFINES

#include <math.h>

#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <functional>

#include <glm/glm.hpp>
#include <glm/detail/precision.hpp>
#include <sstream>

#undef _USE_MATH_DEFINES

constexpr float degreesToRadians(const float angle) {
    return angle * static_cast<float>(M_PI) / 180.0f;
}

template<typename T>
T fpnormalize(T val) {
    if (isnormal(val)) {
        return val;
    } else {
        return 0;
    }
}

template<typename T, glm::precision P, template<typename, glm::precision> class vecType>
GLM_FUNC_QUALIFIER vecType<T, P> normalize_safe(vecType<T, P> const &x) {
    GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'normalize' only accept floating-point inputs");

    auto r = x * glm::inversesqrt(glm::dot(x, x));
    r.x = fpnormalize(r.x);
    r.y = fpnormalize(r.y);
    r.z = fpnormalize(r.z);

    return r;
}

template<typename T, typename E>
bool fuzzEqual(T a, T b, E epsilon) {
    return abs(a - b) <= ((abs(a) < abs(b) ? abs(b) : abs(a)) * epsilon);
}

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    std::not1(std::ptr_fun<int, int>(std::isspace))));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// trim from both ends (copying)
static inline std::string trimmed(std::string s) {
    trim(s);
    return s;
}

std::vector<std::string> split(const std::string &s, char delim);