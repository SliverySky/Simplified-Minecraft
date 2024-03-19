#ifndef WORLD_H
#define WORLD_H
#include "mesh.h"
#include "cubeRender.h"
#include <fstream>
#include <sstream>
struct CubeInfo{
    CubeType type = CubeType::EMPTY;
    std::vector<unsigned int> memory_index;
    int isRender = 0x3f; //六个面是否渲染
};


class World{
public:
    enum CollisionType{
        HITTED,
        OUTOFRANGE,
        NOTHITTED
    };
    const int static MAX_X = 100;
    const int static MAX_Y = 100;
    const int static MAX_Z = 50;
    std::vector<std::vector<std::vector<CubeInfo>>> data;
    Mesh worldMesh;
    CubeRender cubeRender;
    std::vector<unsigned int> indices;
    glm::ivec3 ds[6] = {{0, 1, 0}, {1, 0, 0}, {0, 0, 1}, {0, 0, -1}, {-1, 0, 0}, {0, -1, 0}};


    World(glm::ivec3 lightPos)
    :data(MAX_X, std::vector<std::vector<CubeInfo>>(MAX_Y, std::vector<CubeInfo>(MAX_Z)))
    {       
        std::cout<<"Start to build world."<<std::endl;
        Build(); // 构建世界中的方块数据
        cubeRender.LoadResource(); // 加载方块资源
        worldMesh.textureID = cubeRender.textureID; //方块贴图
        std::cout<<"Start to load vertices data."<<std::endl;
        // 添加Mesh中的顶点数据
        for (int x = 0; x < MAX_X; x++){
        for (int y = 0; y < MAX_Y; y++){
        for (int z  = 0; z < MAX_Z; z++){
            glm::ivec3 pos(x, y, z);
            CubeType curType = data[x][y][z].type;
            if (curType == CubeType::EMPTY) continue;
            AddCube(glm::ivec3(x, y, z), curType, false);
            //std::cout<<x<<" "<<y<<" "<<z <<" "<<std::endl;
        }}}
        std::cout<<"Start to update indices."<<std::endl;
        worldMesh.MemToBuffer();
        UpdateIndices();
        worldMesh.UpdateIndices(indices);
    }
    void Build(){
        for (int x = 0; x < MAX_X; x++)
            for (int y = 0; y < 2; y++)
                for (int z = 0; z < MAX_Z; z++)
                    data[x][y][z].type = CubeType::GROUND;

        //data[lightPos.x][lightPos.y][lightPos.z] = CubeType::WATER;
        // two simple cubes for shadow test
        // data[10][2][10] = CubeType::GRASS;
        // data[8][4][8] = CubeType::GRASS;
        
        std::string file_names[2] = {"./world1.txt", "./world2.txt"};
        for (int i = 0; i < 2; i++){
        std::ifstream file(file_names[i]);
        if(file.is_open()){
            std::string line;
            while(std::getline(file, line)) {
                std::istringstream iss(line);
                std::vector<std::string> tokens;
                int x, y, z, t;
                CubeType t0;
                iss >> x >> y >> z >> t;
                if (t==0) t = CubeType::GROUND;
                else if (t==1) t0 = CubeType::GRASS;
                else if (t==2) t0 = CubeType::TREE;
                else if (t==3) t0 = CubeType::LEAF;
                else if (t==4) t0 = CubeType::FLOWER;
                else if (t==5) t0 = CubeType::WATER;
                else if (t==6) t0 = CubeType::PLANT;
                data[x][y][z].type = t0;
            }
        }else{
            std::cout << "The file can not be opened!" << std::endl;
        }}
    }
    void UpdateIndices(){
        indices.clear();
        for (int x = 0; x < MAX_X; x++){
        for (int y = 0; y < MAX_Y; y++){
        for (int z  = 0; z < MAX_Z; z++){
            glm::ivec3 pos(x, y, z);
            CubeInfo& info = data[x][y][z];
            if (info.type == CubeType::EMPTY) continue;
            if (IsOpaque(glm::ivec3(x, y, z))){
                assert(info.memory_index.size()==36);
                for (int i = 0; i < 6; i++){ // 6个面
                    if (info.isRender & (1<<i)){
                        // 每个面的6个顶点
                        for (int j = 0; j < 6; j++) indices.push_back(info.memory_index[6 * i + j]);
                    }
                }
            }else{
               for(auto i:info.memory_index) indices.push_back(i);
            }
        }}}
        std::cout<<"#indices:"<<indices.size()<<std::endl;
    }


    void AddCube(glm::ivec3 pos, CubeType type, bool instant=true){
        glm::ivec3 p2;
        CubeInfo & info = data[pos.x][pos.y][pos.z];
        info.type = type;
        info.memory_index = cubeRender.AddCubeToMesh(worldMesh, type, pos, instant);
        if(IsOpaque(pos)) UpdateRenderInfo(pos);
        for(int k = 0; k < 6; k++){
            glm::ivec3 p2 = pos + ds[k];
            if(IsInRange(p2) && IsOpaque(p2)) UpdateRenderInfo(p2);
        }
        if(instant){
            UpdateIndices();
            worldMesh.UpdateIndices(indices);
        }
    }

    void RemoveCube(glm::ivec3 pos){
        CubeInfo & info = data[pos.x][pos.y][pos.z];
        info.type = EMPTY;
        worldMesh.RemoveVertices(info.memory_index);
        info.memory_index.clear();
        for(int k = 0; k < 6; k++){
            glm::ivec3 p2 = pos + ds[k];
            if(IsInRange(p2) && IsOpaque(p2)) UpdateRenderInfo(p2);
        }
        UpdateIndices();
        worldMesh.UpdateIndices(indices);
    }

    void UpdateRenderInfo(glm::ivec3 pos){
        CubeInfo& info = data[pos.x][pos.y][pos.z];
        glm::ivec3 p2;
        int isRender = 0x3f;
        for(int k = 0; k < 6; k++){
            p2 = pos + ds[k];
            if(IsInRange(p2) && IsOpaque(p2)){
                isRender = isRender & (~(1<<k));
            }
        }
        info.isRender = isRender;
    }

    void Render(Shader &shader){
        worldMesh.Draw(shader, indices);
    }

    bool IsOpaque(glm::ivec3 pos){
        CubeType type = data[pos.x][pos.y][pos.z].type;
        if ( type != CubeType::EMPTY && type != CubeType::FLOWER && 
             type != CubeType::LEAF && type != CubeType::PLANT)
            return true;
        else
            return false;
    }
    bool IsInRange(glm::ivec3 _pos){
        if(_pos.x >= 0 && _pos.x < MAX_X && _pos.y >= 0 && _pos.y < MAX_Y &&
           _pos.z > 0 && _pos.z < MAX_Z) return true;
        return false;
    }
    CollisionType isCollision(glm::vec3 pos){
        glm::ivec3 _pos(floor(pos.x), floor(pos.y), floor(pos.z));
        if(_pos.x >= 0 && _pos.x < MAX_X && _pos.y >= 0 && _pos.y < MAX_Y &&
                 _pos.z > 0 && _pos.z < MAX_Z){
            if (data[_pos.x][_pos.y][_pos.z].type != CubeType::EMPTY) return HITTED;
            else return NOTHITTED;
        }else{
            return OUTOFRANGE;
        }
    }


};
#endif