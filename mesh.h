#ifndef MESH_H
#define MESH_H
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <glad/glad.h>
#include "shader.h"
struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoords;  
};
struct Texture{
    unsigned int id;
    std::string type;
    std::string path;
};
class Mesh{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int textureID;
    unsigned int VAO;

    //构造函数
    Mesh(){}

    void Draw(Shader &shader){
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT,0);
        glBindVertexArray(0); //解绑，防止VAO被修改
        
    }


private:
    unsigned int VBO, EBO;
    bool isSetup = false;
    /*
        设置VBO和VAO，并且设置顶点属性
    */
public:
    void SetupMesh(){
        if (isSetup){
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDeleteBuffers(1, &VBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glDeleteBuffers(1, &EBO);
            glBindVertexArray(0);
            glDeleteVertexArrays(1, &VAO);
        }
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0); // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(1); //normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2); //texCoords
        glVertexAttribPointer(2, 2, GL_FLOAT,GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));
        isSetup = true;
    }

};

#endif