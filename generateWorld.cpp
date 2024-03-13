#include <fstream>
#include <iostream>
#include <random>
#include <glm/glm.hpp>
void Output(std::ofstream& file,int a,int b,int c,int d){
    file << a<<" " << b <<" "<< c<<" " << d <<std::endl;
}
void GenerateTree(std::ofstream& file, glm::ivec3 pos, int h, int w1, int w2){
    for(int y = 0; y < h; y++){
        for(int x = 0; x < w1 + 2*w2; x++){
        for(int z = 0; z < w1 + 2*w2; z++){
            if(x >= w2 && x < w1 + w2 && z >= w2 && z < w1 + w2){
                Output(file, pos.x+x, pos.y+y, pos.z+z, 2);    
            }else{
                if(y > h / 2 ){
                    Output(file, pos.x+x, pos.y+y, pos.z+z, 6);    
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
int main(){
    std::ofstream file("./world2.txt");
    if(file.is_open()){
        GenerateMoutain(file);
        GenerateTree(file,glm::ivec3(30, 2, 30), 20, 2, 3);
    }else{
        std::cout<<"Error"<<std::endl;
    }

}
