#ifndef MESH_H
#define MESH_H
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <glad/glad.h>
#include "shader.h"
#include <stack>
#include <queue>
#include <iostream>
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
    // std::vector<Vertex> vertices;
    // std::vector<unsigned int> indices;
    unsigned int textureID;
    unsigned int VAO, VBO, EBO;
    int MAX_NUM;
    std::queue<unsigned int> freeSpace;
    std::vector<Vertex> vertex_mem;
    std::vector<unsigned int> indices_mem;
    int indices_num = 0;
    //构造函数
    Mesh(int max_cube_num): MAX_NUM(36 * max_cube_num){
    }

    bool AllocateResource(){
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ARRAY_BUFFER, MAX_NUM * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
        for(int i=0; i<MAX_NUM; i++){
            freeSpace.push(i);
        }
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_NUM * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0); // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(1); //normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2); //texCoords
        glVertexAttribPointer(2, 2, GL_FLOAT,GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        return true;
    }

    void BindBuffer(){
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    }
    void UnbindBuffer(){
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    ~Mesh(){
        UnbindBuffer();
        glDeleteBuffers(1, &VBO);      
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }

    // std::vector<unsigned int> AddVertices(std::vector<Vertex> &vertices){
    // }
    void MemToBuffer(){
        int n = vertex_mem.size();
        // if (n == 0) return;
        BindBuffer();
        for(int i = 0; i < n;){
            int j;
            for(j = i+1; j < n; j++){
                if(indices_mem[j] != indices_mem[j-1] + 1) break;
            }
            glBufferSubData(GL_ARRAY_BUFFER, indices_mem[i] * sizeof(Vertex), (j-i) * sizeof(Vertex), &vertex_mem[i]);
            i = j;
        }
        vertex_mem.clear();
        indices_mem.clear();
        UnbindBuffer();

    }
    std::vector<unsigned int> AddVertices(std::vector<Vertex> &vertices, bool instant=true){
        int n = vertices.size();
        std::vector<unsigned int> indices;
        if(instant){
            BindBuffer();
            for(int i = 0; i < n; i++){
                unsigned int t = freeSpace.front();
                freeSpace.pop();
                glBufferSubData(GL_ARRAY_BUFFER, t * sizeof(Vertex), sizeof(Vertex), &vertices[i]);
                indices.push_back(t);    
            }
            UnbindBuffer();
        }else{
            for(int i = 0; i < n; i++){
                unsigned int t = freeSpace.front();
                freeSpace.pop();
                vertex_mem.push_back(vertices[i]);
                indices_mem.push_back(t);
                indices.push_back(t);
            }
        }
        return indices;
    }

    void RemoveVertices(std::vector<unsigned int> indices){
        for(auto i: indices) freeSpace.push(i);
    }
    void UpdateIndices(std::vector<unsigned int> &indices){
        BindBuffer();
        indices_num = indices.size();
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(unsigned int), &indices[0]);
        UnbindBuffer();
    }
    void Draw(Shader &shader){
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        BindBuffer();
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices_num), GL_UNSIGNED_INT,0);
        UnbindBuffer();
    }
    // void UpdateVertexBuffer(std::vector<int> indices){
    //     for(auto i: indices){
    //         glBufferSubData(GL_ARRAY_BUFFER, i * sizeof(Vertex), sizeof(Vertex), &vertices[i]);
    //     }
    // }

// private:
    
//     //bool isSetup = false;
//     /*
//         设置VBO和VAO，并且设置顶点属性
//     */
// public:
    // void SetupMesh(){

    //     glBindVertexArray(VAO);
    //     glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //     glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //     glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    //     glEnableVertexAttribArray(0); // position
    //     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    //     glEnableVertexAttribArray(1); //normal
    //     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
    //     glEnableVertexAttribArray(2); //texCoords
    //     glVertexAttribPointer(2, 2, GL_FLOAT,GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));
    //     isSetup = true;
    // }

};

#endif