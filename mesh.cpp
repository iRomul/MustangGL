#include "mesh.hpp"

#include <cstdint>
#include <istream>

using namespace std;

bool Mesh::load(istream &is) {

    uint32_t vertexCount = 0;
    is.read((char *) &vertexCount, sizeof(uint64_t));

    data.resize(vertexCount);
    is.read((char *) &data[0], sizeof(Vertex) * vertexCount);

    return true;
}

//рисуем меш
void Mesh::draw() {
    glBegin(GL_TRIANGLES);

    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < data.size(); i++) {
        if (i % 3 == 0) {
            glNormal3f(data[i].n[0], data[i].n[2], -data[i].n[1]);
        }
        glTexCoord2f(data[i].uv[0], data[i].uv[1]);//координаты текстуры
        glVertex3f(data[i].p[0], data[i].p[2], -data[i].p[1]);//координаты вершин
    }

    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
}
