#ifndef CUBERENDER_H
#define CUDERENDER_H
#include <glad/glad.h>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif
#include <string>
#include <iostream>
#include <map>
#include "mesh.h"
#include <glm/glm.hpp>
enum CubeType{
    EMPTY,
    GRASS,
    GROUND,
    TREE,
    LEAF,
    FLOWER,
    PLANT,
    WATER
};
struct Cube{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    bool isCollisible = true;
    bool isOpaque = true;
};
class CubeRender{
public:
    unsigned int textureID;
    std::map<CubeType, Cube> cubeDatabase;
    CubeRender(){}
    void LoadResource(){
        // 加载贴图资源
        textureID = TextureFromFile("./resources/textures/texture.png");
        // 加载Cube
        LoadGrassCube();
        LoadGroundCube();
        LoadFlowerCube();
        LoadLeafCube();
        LoadTreeCube();
        LoadWaterCube();
        LoadPlantCube();
    }


    std::vector<unsigned int> AddCubeToMesh(Mesh& mesh, CubeType type, glm::ivec3 position, bool instant=true){
        std::vector<Vertex> vertices;
        for(int i = 0; i < cubeDatabase[type].vertices.size(); i++){
            Vertex t = cubeDatabase[type].vertices[i];
            t.position += position;
            t.texCoords /= 16.0f;
            vertices.push_back(t);
        }
        return mesh.AddVertices(vertices, instant);
    }
private:
    unsigned int TextureFromFile(const std::string path){
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, numComponents;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &numComponents, 0);
        if (data){
            GLenum format;
            if (numComponents == 1){
                format = GL_RED;
            }else if (numComponents == 3){
                format = GL_RGB;
            }else if (numComponents == 4){
                format = GL_RGBA;
            }
            std::cout<<"format:"<<numComponents<<" "<<width<<" "<<height<<std::endl;
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);          
        }else{
            std::cout<<"Fail to load texture at path: "<<path<<std::endl;
        }
        stbi_image_free(data);
        return textureID;
    }
    void AddPlane(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
     glm::vec3 pos, glm::vec3 uaxis, glm::vec3 vaxis, glm::vec2 texCoord){
        int vertex_num = vertices.size(); //贴图坐标从左上角开始
        glm::vec3 normal = glm::cross(vaxis, uaxis);
        vertices.push_back(Vertex{pos, normal, texCoord});
        vertices.push_back(Vertex{pos + vaxis, normal, texCoord + glm::vec2(0.0f, 1.0f)});
        vertices.push_back(Vertex{pos + vaxis + uaxis, normal, texCoord + glm::vec2(1.0f, 1.0f)});
        vertices.push_back(Vertex{pos + vaxis + uaxis, normal, texCoord + glm::vec2(1.0f, 1.0f)});
        vertices.push_back(Vertex{pos + uaxis, normal,  texCoord + glm::vec2(1.0f, 0.0f)});
        vertices.push_back(Vertex{pos, normal, texCoord}); 
        for (int i = 0; i < 6; i++){
            indices.push_back(vertex_num + i);
        }
    }
    void AddMesh(Cube& cube, glm::vec2 textureCoords[6]){
        std::vector<Vertex>& vertex = cube.vertices;
        std::vector<unsigned int>& indices = cube.indices;
        glm::vec3 xaxis = {1.0f, 0.0f, 0.0f};
        glm::vec3 yaxis = {0.0f, 1.0f, 0.0f};
        glm::vec3 zaxis = {0.0f, 0.0f, 1.0f};
        AddPlane(vertex, indices, glm::vec3(0, 1, 0), xaxis, zaxis, textureCoords[0]);// (0, 1, 0)
        AddPlane(vertex, indices, glm::vec3(1, 1, 1), -zaxis, -yaxis, textureCoords[1]);// (1, 0, 0)
        AddPlane(vertex, indices, glm::vec3(0, 1, 1), xaxis, -yaxis , textureCoords[2]); // (0, 0, 1)
        AddPlane(vertex, indices, glm::vec3(1, 1, 0), -xaxis, -yaxis , textureCoords[3]); // (0, 0, -1)
        AddPlane(vertex, indices, glm::vec3(0, 1, 0), zaxis, -yaxis, textureCoords[4]);// (-1, 0, 0)
        AddPlane(vertex, indices, glm::vec3(0, 0, 0), zaxis, xaxis, textureCoords[5]); // (0, -1, 0)
    }
    void LoadGrassCube(){
        cubeDatabase.emplace(GRASS, Cube());
        glm::vec2 textureCoords[6] = {{0, 0}, {1, 0}, {1, 0},{1, 0},{1, 0}, {2, 0}};
        AddMesh(cubeDatabase[GRASS], textureCoords);
    }
    void LoadGroundCube(){
        cubeDatabase.emplace(GROUND, Cube());
        glm::vec2 textureCoords[6] = {{2,0},{2,0},{2,0},{2,0},{2,0},{2,0}};
        AddMesh(cubeDatabase[GROUND], textureCoords);
    }
    void LoadFlowerCube(){
        cubeDatabase.emplace(FLOWER, Cube());
        glm::vec2 textureCoords[2] = {{10,0},{10,0}};
        std::vector<Vertex>& vertex = cubeDatabase[FLOWER].vertices;
        std::vector<unsigned int>& indices = cubeDatabase[FLOWER].indices;
        glm::vec3 xaxis = {1.0f, 0.0f, 0.0f};
        glm::vec3 yaxis = {0.0f, 1.0f, 0.0f};
        glm::vec3 zaxis = {0.0f, 0.0f, 1.0f};
        AddPlane(vertex, indices, glm::vec3(0.5, 1, 1), -zaxis, -yaxis, textureCoords[0]);// (1, 0, 0)
        AddPlane(vertex, indices, glm::vec3(1, 1, 0.5), -xaxis, -yaxis , textureCoords[1]); // (0, 0, -1)
    }
    void LoadPlantCube(){
        cubeDatabase.emplace(PLANT, Cube());
        glm::vec2 textureCoords[2] = {{11,0},{11,0}};
        std::vector<Vertex>& vertex = cubeDatabase[PLANT].vertices;
        std::vector<unsigned int>& indices = cubeDatabase[PLANT].indices;
        glm::vec3 xaxis = {1.0f, 0.0f, 0.0f};
        glm::vec3 yaxis = {0.0f, 1.0f, 0.0f};
        glm::vec3 zaxis = {0.0f, 0.0f, 1.0f};
        AddPlane(vertex, indices, glm::vec3(0.5, 1, 1), -zaxis, -yaxis, textureCoords[0]);// (1, 0, 0)
        AddPlane(vertex, indices, glm::vec3(1, 1, 0.5), -xaxis, -yaxis , textureCoords[1]); // (0, 0, -1)
    }
    void LoadTreeCube(){
        cubeDatabase.emplace(TREE, Cube());
        glm::vec2 textureCoords[6] = {{5,0},{4,0},{4,0},{4,0},{4,0},{5,0}};
        AddMesh(cubeDatabase[TREE], textureCoords);
    }
    void LoadLeafCube(){
        cubeDatabase.emplace(LEAF, Cube());
        glm::vec2 textureCoords[6] = {{6,0},{6,0},{6,0},{6,0},{6,0},{6,0}};
        AddMesh(cubeDatabase[LEAF], textureCoords);
    }
    void LoadWaterCube(){
        cubeDatabase.emplace(WATER, Cube());
        glm::vec2 textureCoords[6] = {{8,0},{8,0},{8,0},{8,0},{8,0},{8,0}};
        AddMesh(cubeDatabase[WATER], textureCoords);
    }

    

};
#endif