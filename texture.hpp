#pragma once

#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include "windows.h"

const WORD BMP_FILE_SIGNATURE = 0x4D42;

class Texture {
private:
    GLuint glTexture;

    bool _load(FILE *file);

public:
    Texture();

    bool load(const char *fileName);

    void bind();

    ~Texture();
};
