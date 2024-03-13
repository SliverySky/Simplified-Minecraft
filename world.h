#ifndef WORLD_H
#define WORLD_H
#include "mesh.h"
#include "cubeRender.h"
#include <fstream>
#include <sstream>
class World{
public:
    const int static MAX_X = 100;
    const int static MAX_Y = 100;
    const int static MAX_Z = 50;
    CubeType data[MAX_X][MAX_Y][MAX_Z];
    Mesh worldMesh;
    CubeRender cubeRender;
    World(glm::ivec3 lightPos){
        //生成世界
        for (int x = 0; x < MAX_X; x++){
            for (int y = 0; y < MAX_Y; y++){
                for (int z = 0; z < MAX_Z; z++){
                    data[x][y][z] = CubeType::EMPTY;
                }
            }
        }        
        for (int x = 0; x < MAX_X; x++){
            for (int y = 0; y < 2; y++){
                for (int z = 0; z < MAX_Z; z++){
                    data[x][y][z] = CubeType::GROUND;
                }
            }
        }
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
                data[x][y][z] = t0;
            }
        }else{
            std::cout << "The file can not be opened!" << std::endl;
        }}


        //加载资源
        cubeRender.LoadResource();
        std::cout<<"here"<<std::endl;
        glm::ivec3 ds[6] = {{0, 1, 0}, {0, -1, 0}, {-1, 0, 0}, {0, 0, 1}, {1, 0, 0}, {0, 0, -1}};
        for (int x = 0; x < MAX_X; x++){
        for (int y = 0; y < MAX_Y; y++){
        for (int z  = 0; z < MAX_Z; z++){
            glm::ivec3 pos(x, y, z);
            CubeType curType = data[pos.x][pos.y][pos.z];
            if (curType == CubeType::EMPTY) continue;
            int isRender = 0x3f;
            glm::ivec3 p2;
            for (int k = 0; k < 6; k++){
                p2 = pos + ds[k];
                if(p2.x < 0 || p2.x >=MAX_X || p2.y < 0 || p2.y >= MAX_Y ||
                    p2.z < 0 || p2.z >=MAX_Z)continue;
                CubeType tmp = data[p2.x][p2.y][p2.z];
                if( tmp != CubeType::EMPTY && tmp != CubeType::FLOWER && tmp != CubeType::LEAF && tmp != CubeType::PLANT){
                    isRender = isRender & (~(1<<k));
                }
            }
            if(isRender > 0){
                cubeRender.AddToWorldMesh(worldMesh, curType, pos, isRender);
            }
        }}}
        
        //加载网格
        worldMesh.textureID =cubeRender.textureID;
        std::cout<<"#vertices:"<<worldMesh.vertices.size()<<std::endl;
        std::cout<<"#indicies:"<<worldMesh.indices.size()<<std::endl;
        worldMesh.SetupMesh();

    }
    void Render(Shader &shader){
        worldMesh.Draw(shader);
    }

    // bool isCollision(){

    // }

};
#endif