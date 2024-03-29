#include <fstream>
#include <iostream>
#include <random>
#include <glm/glm.hpp>
#include <math.h>
#include "generation/bimap.h"
#include "generation/noise.h"
#include "cubeRender.h"
using std::vector;
static const int MAPSIZE = 200;
const int MapHeight = 60;
struct HeightMap{
    float h[MAPSIZE][MAPSIZE];
};
void Output(std::ofstream& file,int a,int b,int c,int d){
    file << a<<" " << b <<" "<< c<<" " << d <<std::endl;
}
void GenerateTree(vector<vector<vector<int>>> &data, vector<vector<int>>& height, HeightMap &hm, std::mt19937& gen, int a, int b){
    if (hm.h[a][b] <= 0) return;
    int max_h1 = 20, min_h1 = 8;
    int h = std::round(min_h1 + (max_h1 - min_h1) * hm.h[a][b]);
    int w2 = std::max(int(std::round(h * 0.2)), 1);
    std::uniform_int_distribution<int> dis_gen(1, 10);
    int dis = dis_gen(gen);
    dis = dis >=5 ? 1:  dis;
    int w1 = h <=12 ? 1 : 2;
    int pos_y = 1e9;
    for (int i = 0; i < w1 + 2*w2 + 2 * dis; i++){
        for (int j = 0; j < w1 + 2*w2 + + 2 * dis; j++){
            if (a + i >= MAPSIZE) return;
            if (b + j >= MAPSIZE) return;
            if (hm.h[a + i][b + j] <= 0) return;
            else hm.h[a + i][b + j] = -1;
            pos_y = std::min(height[a + i][b + j], pos_y);
        }
    }
    if (h + pos_y >= MapHeight) return;
    for(int y = 0; y < h + 1; y++){
        for(int x = 0; x < w1 + 2*w2; x++){
        for(int z = 0; z < w1 + 2*w2; z++){
            if(x >= w2 && x < w1 + w2 && z >= w2 && z < w1 + w2 && y < h){
                data[x + a + dis][y + pos_y][z + b + dis] = 3;
            }else{
                if(y > std::min(h * 3 / 4, h - 2)){
                    data[x + a + dis][y + pos_y][z + b + dis] = 4;
                }
            }
        }}     
    }
}

void GenerateMoutain(std::ofstream& file){
    const int width = 20;
    const int length = 16;
    const int height = 5;
    const int pos_x = 10;
    const int pos_y = 2;
    const int pos_z = 10;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(1, 100);
    for(int h = 0; h < height; h++){
        for (int w = h; w < width - h; w++){
            for (int l = h; l < length - h; l++){
                Output(file, w + pos_x, h + pos_y, l + pos_z, 1);
                if(w == h || w == (width - h - 1) || l == h || l == (length - h -1)){
                    int r_num = dis(gen);
                    if(r_num<20){
                        Output(file, w + pos_x, h + pos_y + 1, l + pos_z, 6);
                    }
                }
            }
        }
    }
}

glm::ivec4 ObtainColor(float x){
    glm::vec3 c1(35, 158, 0);
    glm::vec3 c2(219, 219, 0);
    glm::vec3 c3(125, 125, 125);
    glm::vec3 c4(255, 255, 255);
    glm::ivec3 res;
    x = std::min(std::max(x, -1.0f), 1.0f);
    if (x <= -0.25){
        res = c1 + (c2 - c1) * (x + 1.0f) / 0.75f;
    }else if (x <= 0.25){
        res = c2 + (c3 - c2) * (x + 0.25f) / 0.5f;
    }else{
        res = c3 + (c4 - c3) * (x - 0.25f) / 0.75f;
    }
    return glm::ivec4(res, 1.0);
}

glm::ivec4 ObtainTreeColor(float x){
    glm::vec3 c1(255, 255, 255);
    glm::vec3 c2(0, 255, 58);
    glm::vec3 c3(0, 120, 22);
    glm::ivec3 res;
    x = std::min(std::max(x, -1.0f), 1.0f);
    if (x <= 0){
        res = c1;
    }else{
        res = c2 + (c3 - c2) * x;
    }

    return glm::ivec4(res, 1.0);
}



void GenerateFlatTerrain(HeightMap &hm){
    Noise TerrainGenerator;
    for (int i = 0; i < MAPSIZE; i++){
        for (int j = 0; j < MAPSIZE; j++){
            float r = TerrainGenerator.Valley(i, j, 0.05, 0.1, 8) - 1.0;
            hm.h[i][j] = r;
        }
    }
}
void GenerateMountainTerrain(HeightMap &hm){
    Noise TerrainGenerator;
    for (int i = 0; i < MAPSIZE; i++){
        for (int j = 0; j < MAPSIZE; j++){
            float r = TerrainGenerator.Ridge(i, j, 0.02, 0.5, 8);
            hm.h[i][j] = r;
        }
    }
}
void GenerateTreeTerrain(HeightMap &hm){
    Noise TerrainGenerator;
    for (int i = 0; i < MAPSIZE; i++){
        for (int j = 0; j < MAPSIZE; j++){
            float r = TerrainGenerator.FBM(i, j, 0.02, 1.0, 8);
            hm.h[i][j] = r;
        }
    }
}
void OutputMountain(){
    HeightMap hm;
    Bitmap map(MAPSIZE, MAPSIZE);
    GenerateMountainTerrain(hm);
    for (int i = 0; i < MAPSIZE; i++){
        for (int j = 0; j < MAPSIZE; j++){
            map.SetPixel(i, j, ObtainColor(hm.h[i][j]));
        }
    }
    map.SaveFile("./mountain.png");
}
void OutputFlat(){
    HeightMap hm;
    Bitmap map(MAPSIZE, MAPSIZE);
    GenerateFlatTerrain(hm);
    for (int i = 0; i < MAPSIZE; i++){
        for (int j = 0; j < MAPSIZE; j++){
            map.SetPixel(i, j, ObtainColor(hm.h[i][j]));
        }
    }
    map.SaveFile("./flat.png");
}


HeightMap OutputTree(){
    HeightMap hm;
    Bitmap map(MAPSIZE, MAPSIZE);
    GenerateTreeTerrain(hm);
    for (int i = 0; i < MAPSIZE; i++){
        for (int j = 0; j < MAPSIZE; j++){
            map.SetPixel(i, j, ObtainTreeColor(hm.h[i][j]));
        }
    }
    map.SaveFile("./tree.png");
    return hm;
}
HeightMap GenerateHeightMap(){
    HeightMap hm1 ,hm2, res;
    Bitmap map(MAPSIZE, MAPSIZE);
    GenerateFlatTerrain(hm1);
    GenerateMountainTerrain(hm2);
    int r1 = 30, r2 = 60;
    for (int i = 0; i < MAPSIZE; i++){
        for (int j = 0; j < MAPSIZE; j++){
            int d = (i - 100) * (i - 100) + (j - 100) * (j - 100);
            float v;
            if (d < r1 * r1){
                v = hm1.h[i][j];
            }else if (d > r2 * r2){
                v = hm2.h[i][j];
            }else{
                v = hm1.h[i][j] + (hm2.h[i][j] - hm1.h[i][j]) * 
                    (std::sqrt(d) - r1) / (r2 - r1);
            }
            res.h[i][j] = v;
            map.SetPixel(i, j, ObtainColor(v));
        }
    }
    map.SaveFile("./heightmap.png");
    return res;
}




void buildWorld(HeightMap& mp){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> grass_flower(1, 100);
    int base = 10;
    vector<vector<vector<int>>> data = vector(MAPSIZE, vector<vector<int>>(MapHeight, vector<int>(MAPSIZE, CubeType::EMPTY)));
    vector<vector<int>> height = vector(MAPSIZE, vector<int>(MAPSIZE, 0));
    for (int i = 0; i < MAPSIZE; i++){
        for (int j = 0; j < MAPSIZE; j++){
            for (int k = 0; k < base; k++){
                data[i][k][j] = CubeType::GROUND;
            }
            int h = 30 * (mp.h[i][j] + 1) / 2 + 1;
            for (int k = base; k < base + h; k++){
                data[i][k][j] = CubeType::GROUND;
            }
            data[i][base + h][j] = CubeType::GRASS;
            int t = grass_flower(gen);
            height[i][j] = base + h + 1;
            if (t <= 4){
                data[i][base + h + 1][j] = CubeType::FLOWER;
            }else if (t <= 10){
                data[i][base + h + 1][j] = CubeType::PLANT;
            }
        }
    }

    // generate tree

    const int TREE_NUM = 50;
    HeightMap tree_hm = OutputTree();
    int tree_cnt = 0;
    for (int i = 0; i < MAPSIZE; i++){
        for (int j = 0; j < MAPSIZE; j++){
            GenerateTree(data,height, tree_hm, gen, i, j);      
        }
    }
    std::ofstream file("./worldData.txt");
    if(file.is_open()){
        file << MAPSIZE << " " << MapHeight << " " << MAPSIZE << std::endl;
        for (int x = 0; x < MAPSIZE; x++){
            for (int y = 0; y < MapHeight; y++){
                for (int z = 0; z < MAPSIZE; z++){
                    file << data[x][y][z] <<" ";
                }
            }   
        }
        file << std::endl;
    }else{
        std::cout<<"Error"<<std::endl;
    }
}

int main(){
    OutputMountain();
    OutputFlat();
    HeightMap mp = GenerateHeightMap();
    buildWorld(mp);
    std::cout<<"update";
    //OutputTree();


    // std::ofstream file("./world2.txt");
    // if(file.is_open()){
    //     GenerateMoutain(file);
    //     GenerateTree(file,glm::ivec3(30, 2, 30), 20, 2, 3);
    // }else{
    //     std::cout<<"Error"<<std::endl;
    // }

    // for (int i = 0; i < MAPSIZE; i++){
    //     for (int j = 0; j < MAPSIZE; j++){
    //         // float r = TerrainGenerator.Noise2D(i * eps, j * eps);
    //         //int a = std::round(255.0 * ((r + 1) / 2));
    //         map.SetPixel(i, j, ObtainColor(r));
    //         // glm::ivec4 color;
    //         // if (r < 0.5){
    //         //     color = glm::ivec4(0, 0, a * 2 , 1.0);
    //         // }else if(r < 0.9){
    //         //     color = glm::ivec4(0, a , std::round(a * 0.5), 1.0);
    //         // }else{
    //         //     color = glm::ivec4(0, 0 , 0, 1.0);
    //         // }
    //         // map.SetPixel(i, j, color);
    //     }
    // }
    
}





