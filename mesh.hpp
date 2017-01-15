#pragma once

#include "texture.hpp"
#include <vector>

using namespace std;

struct alignas(1) Vertex {
    float p[3];
    float n[3];
    float uv[2];
};

class Mesh {
private:
    vector<Vertex> data;
public:
    bool load(istream &is);

    bool loadObj(istream &is);

    void draw();
};
