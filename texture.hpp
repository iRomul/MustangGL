#pragma once

#include "types.h"
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>

#pragma pack (1)
class BMPheader{
	public:

      u16   bfType;
      i32    bfSize;
      i32    bfReserved;
      i32    bfOffBits;
      u32    biSize;
      u32    biWidth;
      u32    biHeight;
      u16   biPlanes;
      u16   biBitCount;
      u32    biCompression;
      u32    biSizeImage;
      u32    biXPelsPerMeter;
      u32    biYPelsPerMeter;
      u32    biClrUsed;
      u32    biClrImportant;
};
#pragma pack (0)

class Texture{
private:
	GLuint glTexture;

	bool _load(FILE* file);

public:
	Texture() ;

	bool load(const char* fileName);

	void bind();

	~Texture();
};
