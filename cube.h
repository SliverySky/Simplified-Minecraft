#ifndef CUBE_H
#define CUBE_H
#include "mesh.h"
#include <vector>
class Cube{
public:
    enum CubeType{
        EMPTY,
        GRASS,
        GROUND
    };
    CubeType type;
    bool isCollisible=false;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;


    Cube(CubeType _type=EMPTY){
        type = _type;
        if (type == EMPTY){
            return;
        }
        glm::vec3 xaxis = {1.0f, 0.0f, 0.0f};
        glm::vec3 yaxis = {0.0f, 1.0f, 0.0f};
        glm::vec3 zaxis = {0.0f, 0.0f, 1.0f};
        glm::vec2 texCoords[6];
        
        if (type == GRASS){
            int _texCoords[6][2] = {{2, 0}, {0, 0}, {1, 0}, {1, 0},{1, 0},{1, 0}};
            for(int i = 0; i < 6; i++){
                texCoords[i] = glm::vec2(_texCoords[i][0] , _texCoords[i][1]);
            }
        }else if (type == GROUND){
            int _texCoords[6][2] = {{2, 0}, {2, 0}, {2, 0}, {2, 0},{2, 0},{2, 0}};
            for(int i = 0; i < 6; i++){
                texCoords[i] = glm::vec2(1.0f * _texCoords[i][0], 1.0f * _texCoords[i][1]);
            }
        }
        AddPlane(glm::vec3(0, 0, 0), yaxis, xaxis , texCoords[0]); 
        AddPlane(glm::vec3(0, 0, 1), yaxis, xaxis , texCoords[1]); 
        AddPlane(glm::vec3(0, 0, 0), zaxis, xaxis, texCoords[2]);
        AddPlane(glm::vec3(0, 0, 0), yaxis, zaxis, texCoords[3]);
        AddPlane(glm::vec3(1, 1, 0), -yaxis, zaxis, texCoords[4]);
        AddPlane(glm::vec3(1, 1, 0), zaxis, -xaxis, texCoords[5]);
    }

    void Add(glm::vec3 position, std::vector<Vertex>& _vertices, std::vector<unsigned int>& _indices){
        unsigned int vertex_num = _vertices.size();
        unsigned int index_num = _indices.size();

        _vertices.insert(_vertices.end(), vertices.begin(), vertices.end());
        _indices.insert(_indices.end(), indices.begin(), indices.end());
        for(auto it = _vertices.begin() + vertex_num; it != _vertices.end(); it++){
            it->position += position;
            it->texCoords /= 16.0f; //归一化纹理坐标
           // std::cout<<it->position.x <<" "<<it->position.y<<" "<<it->position.z<<std::endl;
         //  std::cout<<it->texCoords.x <<" "<<it->texCoords.y << std::endl;
        //    std::cout<<typeid(it->position.x).name() <<" "<<typeid(it->normal.x).name() << " "<<typeid(it->texCoords.x).name()<<std::endl;
           
        }
        for(auto it = _indices.begin() + index_num; it != _indices.end(); it++){
            *it += vertex_num; 
            std::cout<<*it<<std::endl;
        }        
    }



private:
    void AddPlane(glm::vec3 pos, glm::vec3 xaxis, glm::vec3 yaxis, glm::vec2 texCoord){
        int vertex_num = vertices.size();
        glm::vec3 normal = glm::cross(xaxis, yaxis);
        vertices.push_back(Vertex{pos, normal, texCoord});
        vertices.push_back(Vertex{pos + xaxis, normal, texCoord + glm::vec2(1.0f, 0.0f)});
        vertices.push_back(Vertex{pos + yaxis, normal, texCoord + glm::vec2(0.0f, 1.0f)});
        vertices.push_back(Vertex{pos + xaxis + yaxis, normal, texCoord + glm::vec2(1.0f, 1.0f)});
        vertices.push_back(Vertex{pos + yaxis, normal,  texCoord + glm::vec2(0.0f, 1.0f)});
        vertices.push_back(Vertex{pos + xaxis, normal, texCoord + glm::vec2(1.0f, 0.0f)});   
        
        for (int i = 0; i < 6; i++){
            indices.push_back(vertex_num + i);
        }
    }

};

#endif