#include "texture.hpp"

#include <iostream>
#include <fstream>
#include <windows.h>
#include <bits/unique_ptr.h>

using namespace std;

Texture::Texture(std::string filename) {
    this->filename = filename;
    glTexture = 0;
}

bool Texture::load() {
    return load(filename);
}

//загрузка текстуры
bool Texture::load(std::string filename) {
    ifstream file;
    file.exceptions(ios::failbit | ios::badbit);

    try {
        file.open(filename, ios::in | ios::binary);

        BITMAPFILEHEADER file_header;
        BITMAPINFOHEADER header;

        file.seekg(0, ios::beg);

        file.read((char *) &file_header, sizeof(BITMAPFILEHEADER));

        if (file_header.bfType != BMP_FILE_SIGNATURE) {
            throw header_mismatch_exception("File header mismatch");
        }

        //чтение хедера
        file.read((char *) &header, sizeof(BITMAPINFOHEADER));
        //строчка в изображении
        unique_ptr<uint8_t[]> row(new uint8_t[header.biWidth * 3]);
        //все изображение
        unique_ptr<uint8_t[]> buffer(new uint8_t[header.biWidth * header.biHeight * 3]);

        int width = header.biWidth;
        int height = header.biHeight;

        //чтение картинки
        for (uint32_t y = 0; y < height; y++) {
            file.read((char *) row.get(), width * 3);
            //BGR -> RGB - изобретение мастдаевского гения
            for (uint32_t x = 0; x < width; x++) {
                buffer[y * width * 3 + x * 3 + 0] = row[x * 3 + 2];
                buffer[y * width * 3 + x * 3 + 1] = row[x * 3 + 1];
                buffer[y * width * 3 + x * 3 + 2] = row[x * 3 + 0];
            }
        }

        glGenTextures(1, &glTexture);
        glBindTexture(GL_TEXTURE_2D, glTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer.get());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } catch (exception &e) {
        cerr << "Texture load: can't load bitmap '" << filename << "', cause: " << e.what() << endl;
        return false;
    }

    return true;
}

void Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, glTexture);
}

//удаляем текстуру в деструкторе, если она была загружена
Texture::~Texture() {
    if (glTexture) {
        glDeleteTextures(1, &glTexture);
        glTexture = 0;
    }
}