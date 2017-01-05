#include "texture.hpp"
#include <iostream>

using namespace std;

bool Texture::_load(FILE *file) {
    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER header;

    if (fread(&file_header, sizeof(BITMAPFILEHEADER), 1, file) != 1) {
        return false;
    }

    if (file_header.bfType != BMP_FILE_SIGNATURE) {
        return false;
    }

    //чтение хедера
    if (fread(&header, sizeof(BITMAPINFOHEADER), 1, file) != 1) {
        return false;
    }

    //строчка в изображении
    uint8_t *row = (uint8_t *) malloc(header.biWidth * 3);

    if (!row) {
        return false;
    }

    //все изображение
    uint8_t *buffer = (uint8_t *) malloc(header.biWidth * header.biHeight * 3);

    if (!buffer) {
        free(row);
        return false;
    }

    //чтение картинки
    for (uint32_t y = 0; y < header.biHeight; y++) {
        if (fread(row, 3, header.biWidth, file) != header.biWidth) {
            free(row);//прекрасные ++ без нормального RAII, охх, какой ужас. Насколько же идиотска идея С++, в котором нет RAII по "стандарту"
            free(buffer);//хоть STL есть, идея которой появилась у автора, когда он лежал в больнице после отравления =)
            return false;
        }

        //BGR -> RGB - изобретение мастдаевского гения
        for (uint32_t x = 0; x < header.biWidth; x++) {
            buffer[y * header.biWidth * 3 + x * 3 + 0] = row[x * 3 + 2];
            buffer[y * header.biWidth * 3 + x * 3 + 1] = row[x * 3 + 1];
            buffer[y * header.biWidth * 3 + x * 3 + 2] = row[x * 3 + 0];
        }
    }


    glGenTextures(1, &glTexture);
    glBindTexture(GL_TEXTURE_2D, glTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, header.biWidth, header.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(row);
    free(buffer);

    return true;
}

Texture::Texture() {
    glTexture = 0;
}

//загрузка текстуры
bool Texture::load(const char *fileName) {
    FILE *file = fopen(fileName, "rb");
    if (!file) {
        cout << "can not read file" << endl;
        return false;
    }

    bool result = _load(file);

    fclose(file);

    return result;
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
