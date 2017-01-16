#include "mesh.hpp"

#include <istream>
#include <sstream>
#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iomanip>

#include "utilities.h"

using namespace std;

bool Mesh::load(istream &is) {
    uint32_t vertexCount = 0;
    is.read((char *) &vertexCount, sizeof(uint64_t));

    data.resize(vertexCount);
    is.read((char *) &data[0], sizeof(Vertex) * vertexCount);

    return true;
}

bool Mesh::loadObj(istream &is) {
    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;

    const int OBJ_VERTEX_INDEX = 0;
    const int OBJ_NORMAL_INDEX = 2;

    string line;
    int lineNumber = 0;

    auto nlog = [&]() -> ostream & { return (clog << lineNumber << ": "); };

    while (getline(is, line)) {
        ++lineNumber;

        istringstream iss(line);
        string command;

        iss >> command;
        if (command == "#" || trimmed(line).empty()) {
            /* Comments and empty lines */
            continue;
        } else if (command == "v") {
            /* Vertices */
            glm::vec3 vertex;
            iss >> vertex.x;
            iss >> vertex.y;
            iss >> vertex.z;

            vertices.push_back(vertex);

        } else if (command == "vn") {
            /* Normals */
            glm::vec3 normal;
            iss >> normal.x;
            iss >> normal.y;
            iss >> normal.z;

            normals.push_back(normal);

        } else if (command == "vp") {
            /* Texture coordinates */
            nlog() << "texture coordinate ignored" << endl;
        } else if (command == "g" || command == "o") {
            /* Objects and groups */
            nlog() << "object or group ignored" << endl;
        } else if (command == "f") {
            /* Faces (this version support only triangles) */
            string tmp;
            vector<string> faceVerticesData;

            while (iss >> tmp) {
                faceVerticesData.push_back(tmp);
            }

            if (faceVerticesData.size() != 3) {
                nlog() << "error: only triangles supported" << endl;
                return false;
            }

            for (auto vertex : faceVerticesData) {
                auto parts = split(vertex, '/');

                int vertexIndex = stoi(parts[OBJ_VERTEX_INDEX]) - 1;
                glm::vec3 coords = vertices[vertexIndex];

                int normalIndex = stoi(parts[OBJ_NORMAL_INDEX]) - 1;
                glm::vec3 normal = normals[normalIndex];

                Vertex v;
                v.p[0] = coords.x;
                v.p[1] = coords.y;
                v.p[2] = coords.z;

                v.n[0] = normal.x;
                v.n[1] = normal.y;
                v.n[2] = normal.z;

                data.push_back(v);
            }
        } else {
            nlog() << "unsupported command '" << command << "'" << endl;
            clog << "\t full text: '" << line << "'" << endl;
            return false;
        }
    }

    return true;
}

//рисуем меш
void Mesh::draw() {
    glBegin(GL_TRIANGLES);

    for (int i = 0; i < data.size(); i++) {
        if (i % 3 == 0) {
            glNormal3f(data[i].n[0], data[i].n[2], -data[i].n[1]);
        }
        glVertex3f(data[i].p[0], data[i].p[2], -data[i].p[1]);//координаты вершин
    }

    glEnd();
}
