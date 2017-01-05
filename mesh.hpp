#pragma once

#include "texture.hpp"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

#pragma pack (1)
struct Vertex{
	float p[3];
	float n[3];
	float uv[2];
};
#pragma pack (0)

class Mesh{
private:
    vector<Vertex> data;
    Texture* texture;
public:
    Mesh();

    bool load(FILE* file, Texture* texture);

    void draw();
};
