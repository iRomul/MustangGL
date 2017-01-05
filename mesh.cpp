
#include "types.h"
#include "mesh.hpp"

    Mesh::Mesh(){
        texture=0;
    }

    bool Mesh::load(FILE* file, Texture* useTexture){
        texture=useTexture;

        u64 vertexCount=0;
    	if( fread(&vertexCount, sizeof(u64), 1, file)!=1 ) return false;

    	data.resize(vertexCount);
    	if( fread(&data[0], sizeof(Vertex), vertexCount, file)!=vertexCount ){
    		return false;
    	}

    	return true;
    }

    //рисуем меш
    void Mesh::draw(){
        if(texture!=0) {
            //биндим текстуру
            texture->bind();
        }else{
            glBindTexture(GL_TEXTURE_2D,0);
        }

    	glBegin(GL_TRIANGLES);

    	glColor3f(1.0f,1.0f,1.0f);
    	for (int i=0;i<data.size();i++){
			if( i%3==0 ){
				glNormal3f( data[i].n[0], data[i].n[2], -data[i].n[1]);
			}
    		glTexCoord2f( data[i].uv[0], data[i].uv[1]);//координаты текстуры
    		glVertex3f( data[i].p[0], data[i].p[2], -data[i].p[1]);//координаты вершин
    	}

    	glEnd();

    	glBindTexture(GL_TEXTURE_2D,0);
    }
