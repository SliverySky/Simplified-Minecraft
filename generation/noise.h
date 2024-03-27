/*
    These codes are modified from "https://rtouti.github.io/graphics/perlin-noise-algorithm".
    and "https://libnoise.sourceforge.net/".
*/

#ifndef NOISE_H
#define NOISE_H
#include <vector>
#include <algorithm>
#include <random>
#include <glm/glm.hpp>
#include <iostream>
class Noise{
public:
    
    static const int SEED_NUM = 20;
    std::vector<int> permutations[SEED_NUM];
    unsigned int seed;
    Noise():
    seed(0)
    {
        for (int i = 0; i < SEED_NUM; i++){
            permutations[i] = GetPermute();
        }
    }
    inline float Fade(float x){
        return ((6 * x - 15) * x + 10) * x * x *x;
    }

    inline float Lerp(float t, float a1, float a2){
        return a1 + t * (a2 - a1);
    }

    std::vector<int> GetPermute(){
        std::vector<int> t;
        std::random_device rd;
        std::mt19937 g(rd());
        for (int i = 0; i < 256; i++){
            t.push_back(i);
        }
        std::shuffle(t.begin(), t.end(), g);
        for (int i = 0; i < 256; i++){
            t.push_back(t[i]);
        }
        return t;
    }

    glm::vec2 GetConstantVector(int x){
        const int h = x & 3;
        if (h == 0) return glm::vec2(1.0, 1.0);
        else if(h == 1)
            return glm::vec2(-1.0, 1.0);
        else if(h == 2)
            return glm::vec2(-1.0, -1.0);
        else
            return glm::vec2(1.0, -1.0);
    }

    float FBM (float x, float y, float f, float a, int N = 8){
        float r = 0;
        for (int o = 0; o < N; o++){
            seed = 0;
            r += a * Noise2D(x * f, y * f);
            a *= 0.5;
            f *= 2.0;
        }
        return r;
    }

    float Valley (float x, float y, float f, float a, int N = 8){
        float r = 0;
        for (int o = 0; o < N; o++){
            seed = o;
            float t = Noise2D(x * f, y * f);
            r += a * (std::fabs(t) * 2.0 - 1.0);
            a *= 0.5;
            f *= 2.0;
        }
        r += 0.5;
        return r;        
    }

    float Ridge (float x, float y, float f, float a, int N = 8){
        float r = 0;
        for (int o = 0; o < N; o++){
            seed = o;
            float t = Noise2D(x * f, y * f);
            t = 1.0f - std::fabs(t);
            t = t * t;
            r += a * (-std::fabs(t) * 2.0 + 1.0);
            a *= 0.5;
            f *= 2.0;
        }
        return r;        
    }
    float Noise2D (float x, float y){
        std::vector<int>& permutation = permutations[seed % SEED_NUM];
        const int X = int(std::floor(x)) & 255;
        const int Y = int(std::floor(y)) & 255;

        const float xf = x - std::floor(x);
        const float yf = y - std::floor(y);

        const glm::vec2 topRight (xf - 1.0, yf - 1.0);
        const glm::vec2 topLeft (xf, yf - 1.0);
        const glm::vec2 bottomRight (xf - 1.0, yf);
        const glm::vec2 bottomLeft (xf, yf);
        
        // Select a value from the permutation array for each of the 4 corners
        const int valueTopRight = permutation[permutation[X+1] + Y + 1];
        const int valueTopLeft = permutation[permutation[X] + Y + 1];
        const int valueBottomRight = permutation[permutation[X + 1] + Y];
        const int valueBottomLeft = permutation[permutation[X] + Y];
        
        const float dotTopRight = glm::dot(topRight, GetConstantVector(valueTopRight));
        const float dotTopLeft = glm::dot(topLeft, GetConstantVector(valueTopLeft));
        const float dotBottomRight = glm::dot(bottomRight, GetConstantVector(valueBottomRight));
        const float dotBottomLeft = glm::dot(bottomLeft, GetConstantVector(valueBottomLeft));
        const float u = Fade(xf);
        const float v = Fade(yf);
        return Lerp(u,
            Lerp(v, dotBottomLeft, dotTopLeft),
            Lerp(v, dotBottomRight, dotTopRight)
        );    
    }

};

#endif