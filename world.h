#ifndef WORLD_H
#define WORLD_H
#include "mesh.h"
#include "cubeRender.h"
#include <fstream>
#include <sstream>
using std::vector;
struct CubeInfo{
    CubeType type = CubeType::EMPTY;
    vector<unsigned int> memory_index;
    int isRender = 0x3f; //六个面是否渲染
};


class World{
public:
    enum CollisionType{
        HITTED,
        OUTOFRANGE,
        NOTHITTED
    };
    const int static MAX_X = 200;
    const int static MAX_Y = 80;
    const int static MAX_Z = 200;
    const int static CHUNK_SIZE = 10;
    const int static CHUNK_X = MAX_X / CHUNK_SIZE;
    const int static CHUNK_Y = MAX_Y / CHUNK_SIZE;
    const int static CHUNK_Z = MAX_Z / CHUNK_SIZE;
    const int static CUBE_PER_CHUNK = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
    vector<vector<vector<CubeInfo>>> data;
    
    CubeRender cubeRender;
    glm::ivec3 ds[6] = {{0, 1, 0}, {1, 0, 0}, {0, 0, 1}, {0, 0, -1}, {-1, 0, 0}, {0, -1, 0}};
    struct Chunks{
    private:
        vector<vector<vector<Mesh>>> data;
        vector<vector<vector<vector<unsigned int>>>> indices;
    public:
        vector<glm::ivec3> ks;
        Chunks():
        data(CHUNK_X, vector<vector<Mesh>>(CHUNK_Y, vector<Mesh>(CHUNK_Z, Mesh(CUBE_PER_CHUNK)))),
        indices(CHUNK_X, vector<vector<vector<unsigned int>>>(CHUNK_Y, vector<vector<unsigned int>>(CHUNK_Z, vector<unsigned int>(0))))
        {
            for (int x = 0; x < CHUNK_X; x++){
            for (int y = 0; y < CHUNK_Y; y++){
            for (int z = 0; z < CHUNK_Z; z++){
                ks.push_back({x, y, z});
                data[x][y][z].AllocateResource();
            }}}
        }
        Mesh &operator[](const glm::ivec3& i){
            return data[i.x][i.y][i.z];
        }
        void ClearIndices(){
            for (auto k: ks){
                indices[k.x][k.y][k.z].clear();
            }
        }
        void AddIndex(const glm::ivec3& cubePos, int index){
            glm::ivec3 pos = cubePos / CHUNK_SIZE;
            indices[pos.x][pos.y][pos.z].push_back(index);
        }
        void UpdateIndices(){
            for (auto k: ks){
                data[k.x][k.y][k.z].UpdateIndices(indices[k.x][k.y][k.z]);
            }
        }
        int GetIndicesNum(){
            int sum = 0;
            for (auto k: ks){
                sum += indices[k.x][k.y][k.z].size();
            }
            return sum;
        }
    }chunks;

    World(glm::ivec3 lightPos)
    :data(MAX_X, vector<vector<CubeInfo>>(MAX_Y, vector<CubeInfo>(MAX_Z)))
    {       
        std::cout<<"Start to build world."<<std::endl;
        Build(); // 构建世界中的方块数据
        cubeRender.LoadResource(); // 加载方块资源
        //设置方块贴图
        for(auto i: chunks.ks){
            chunks[i].textureID = cubeRender.textureID;
        }
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
        for(auto i: chunks.ks){
            chunks[i].MemToBuffer();
        }
        UpdateChunkIndices();
    }
    void Build(){
        for (int x = 0; x < MAX_X; x++)
            for (int y = 0; y < 2; y++)
                for (int z = 0; z < MAX_Z; z++)
                    data[x][y][z].type = CubeType::EMPTY;

        //data[lightPos.x][lightPos.y][lightPos.z] = CubeType::WATER;
        // two simple cubes for shadow test
        // data[10][2][10] = CubeType::GRASS;
        // data[8][4][8] = CubeType::GRASS;
        
        std::string file_name = "./worldData.txt";
        std::ifstream file(file_name);
        if(file.is_open()){
            std::string line;
            std::getline(file, line);
            std::istringstream iss(line);
            int sizX, sizY, sizZ;
            iss >> sizX >> sizY >> sizZ;
            assert(sizX <= MAX_X && sizY <= MAX_Y && sizZ <= MAX_Z);
            std::getline(file, line);
            iss.clear();
            iss.str(line);
            for (int x = 0; x < sizX; x++){
                for (int y = 0; y < sizY; y++){
                    for (int z = 0; z < sizZ; z++){
                        int ct;
                        iss >> ct;
                        data[x][y][z].type = static_cast<CubeType>(ct);
                    }
                }
            }
        }else{
            std::cout << "The file can not be opened!" << std::endl;
        }
    }
    void UpdateChunkIndices(){
        chunks.ClearIndices();
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
                        for (int j = 0; j < 6; j++) 
                            chunks.AddIndex(pos, info.memory_index[6 * i + j]);
                    }
                }
            }else{
               for(auto i:info.memory_index)
                    chunks.AddIndex(pos, i);
            }
        }}}
        chunks.UpdateIndices();
        //std::cout<<"#indices:"<<chunks.GetIndicesNum()<<std::endl;
    }


    void AddCube(glm::ivec3 pos, CubeType type, bool instant=true){
        glm::ivec3 p2;
        CubeInfo & info = data[pos.x][pos.y][pos.z];
        glm::ivec3 cIndex = pos / CHUNK_SIZE;
        info.type = type;
        //std::cout<<cIndex.x<<" "<<cIndex.y<<" "<<cIndex.z<<" "<<"add cube"<<std::endl;
        info.memory_index = cubeRender.AddCubeToMesh(chunks[cIndex], type, pos, instant);
        //std::cout<<"end cube"<<std::endl;
        if(IsOpaque(pos)) UpdateRenderInfo(pos);
        for(int k = 0; k < 6; k++){
            glm::ivec3 p2 = pos + ds[k];
            if(IsInRange(p2) && IsOpaque(p2)) UpdateRenderInfo(p2);
        }
        if(instant){
            UpdateChunkIndices();
        }
    }

    void RemoveCube(glm::ivec3 pos){
        CubeInfo & info = data[pos.x][pos.y][pos.z];
        info.type = EMPTY;
        glm::ivec3 cIndex = pos / CHUNK_SIZE;
        chunks[cIndex].RemoveVertices(info.memory_index);
        info.memory_index.clear();
        for(int k = 0; k < 6; k++){
            glm::ivec3 p2 = pos + ds[k];
            if(IsInRange(p2) && IsOpaque(p2)) UpdateRenderInfo(p2);
        }
        UpdateChunkIndices();
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

    void Render(Shader &shader, Camera &camera, bool IsShadow=false){
        int cnt = 0;
        for (auto k: chunks.ks){
            if(IsShadow)
                chunks[k].Draw(shader);
            else{
                glm::vec3 p1 (k.x * CHUNK_SIZE, k.y * CHUNK_SIZE, k.z * CHUNK_SIZE);
                glm::vec3 p2 = p1 + float(CHUNK_SIZE);
                if (camera.IsInFrustum(p1, p2)){
                    chunks[k].Draw(shader);
                    cnt += 1;
                }
            }
            
        }
        //chunks[glm::ivec3(0,0,0)].Draw(shader);
        // if (!IsShadow)
        //     std::cout<<"chunks:"<<cnt<<"/"<<chunks.ks.size()<<std::endl;
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
    bool IsEmpty(glm::ivec3 pos){
        return data[pos.x][pos.y][pos.z].type == CubeType::EMPTY;
    }
    // 判断所在区域是否为空，并且周围有方块
    bool IsBuildable(glm::vec3 pos){
        glm::ivec3 ipos(floor(pos.x), floor(pos.y), floor(pos.z));
        if(IsInRange(ipos)){
            bool isEmpty = IsEmpty(ipos);
            bool isNeighborFull = false;
            for (int i = 0; i < 6; i++){
                glm::ivec3 t = ipos + ds[i];
                if (IsInRange(t) && !IsEmpty(t)){
                    isNeighborFull = true;
                    break;
                }
            }
            return isEmpty && isNeighborFull;
        }else{
            return false;
        }       
    }
    // 判断所在区域是否有方块
    bool IsRemoveAble(glm::vec3 pos){
        return IsCollision(pos) == HITTED;
    }
    CollisionType IsCollision(glm::vec3 pos){
        glm::ivec3 _pos(floor(pos.x), floor(pos.y), floor(pos.z));
        if (IsInRange(_pos)){
            if (data[_pos.x][_pos.y][_pos.z].type != CubeType::EMPTY) 
                return HITTED;
            else 
                return NOTHITTED;
        }else{
            return OUTOFRANGE;
        }
    }
};
#endif