#pragma once

#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <windef.h>
#include <string>
#include <exception>

const WORD BMP_FILE_SIGNATURE = 0x4D42;

class header_mismatch_exception : public std::exception {
    std::string msg;

public:
    header_mismatch_exception(std::string msg) {
        this->msg = msg;
    }

    const char *what() const {
        return msg.c_str();
    }
};

class Texture {
private:
    std::string filename;
    GLuint glTexture;

public:
    Texture(std::string filename);

    bool load();
    bool load(std::string filename);

    void bind();

    ~Texture();
};
