#ifndef BUILDTOOL_H
#define BUILDTOOL_H
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include "world.h"
#include <vector>
#include <functional>
class BuildTool{
public:
    sf::RectangleShape verticalLine;
    sf::RectangleShape horizontalLine;
    unsigned int VAO;
    unsigned int VBO;
    Shader shader;
    std::vector<float> vertices;
    bool isHit=false;
    glm::ivec3 hitPos;
    float accumTime = 0;
    sf::Text text;
    sf::Font font;
    sf::RectangleShape textBox;
    BuildTool():
    verticalLine(sf::Vector2f(3,30)), 
    horizontalLine(sf::Vector2f(30,3)),
    shader("./shader/line.vs", "./Shader/line.fs"),
    textBox(sf::Vector2f(200, 50))
    {
        verticalLine.setFillColor(sf::Color::White);
        horizontalLine.setFillColor(sf::Color::White);
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // 添加顶点信息 24 * 3 float
        for(int i = 0; i < 8; i++){
            float a = (i & 0x1), b = ((i&0x2)>>1), c = ((i&0x4)>>2);
            if (a) vertices.insert(vertices.end(), {a, b, c, a - 1, b, c});
            if (b) vertices.insert(vertices.end(), {a, b, c, a, b-1, c});
            if (c) vertices.insert(vertices.end(), {a, b, c, a, b, c - 1});
        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 72, &vertices[0], GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        if (!font.loadFromFile("./font/STSONG.TTF")) {
            throw("load font fail");
        }
        text.setFont(font);
        text.setString(L"移除模式");
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::Red);
        text.setPosition(600, 10);
        
        textBox.setFillColor(sf::Color::White);
        textBox.setOutlineColor(sf::Color::Black);
        textBox.setOutlineThickness(2);
        textBox.setPosition(590, 10);
    }
    void Work(sf::RenderWindow &window, Camera &camera, World &world, float deltaTime){
        float centerX = window.getSize().x / 2;
        float centerY = window.getSize().y / 2;
        verticalLine.setPosition(centerX - 1.5, centerY - 15);
        horizontalLine.setPosition(centerX - 15, centerY - 1.5);
        window.pushGLStates();
        window.draw(verticalLine);
        window.draw(horizontalLine);
        window.draw(textBox);
        window.draw(text);
        window.popGLStates();
        glm::ivec3 _hitPos;
        bool isHit = HitCube(camera.position, camera.front, world, _hitPos);
        if (!isHit){
            accumTime = 0;
            return;
        } 
        if (_hitPos != hitPos){
            accumTime = 0;
            hitPos = _hitPos;
        }
        //std::cout<<hitPos.x <<" "<<hitPos.y<<" "<<hitPos.z<<" "<<std::endl;
        shader.use();
        shader.setMat4("projection", camera.projMat);
        shader.setMat4("view", camera.getViewMatrix());
        std::vector<float> _vertices(vertices);
        for (int i = 0; i < 24; i++){
            _vertices[3 * i] += hitPos.x;
            _vertices[3 * i + 1] += hitPos.y;
            _vertices[3 * i + 2] += hitPos.z;
        }
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glLineWidth(6.0f);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) *72, &_vertices[0]);
        glDrawArrays(GL_LINES, 0, 24);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
            accumTime += deltaTime;
        }else{
            accumTime = 0;
        }
        if(accumTime > 0.7){
            world.RemoveCube(hitPos);
        }
    }

    void HandEvent(sf::Event event, float deltaTime){
    }
    bool HitCube(glm::vec3 source, glm::vec3 front, World &world, glm::ivec3 &hitPos){
        float eps = 1e-2;
        glm::vec3 epsVec(front.x >= 0 ? eps : -eps, front.y >= 0 ?
                     eps : -eps, front.z >= 0 ? eps: -eps);
        std::function<float(float)> _ceil = [](float x){return ceil(x);};
        std::function<float(float)> _floor =[](float x){return floor(x);};
        std::function<float(float)> Fx = front.x >=0 ? _ceil : _floor;
        std::function<float(float)> Fy = front.y >=0 ? _ceil : _floor;
        std::function<float(float)> Fz = front.z >=0 ? _ceil : _floor;

        float x, y, z, mint;
        glm::vec3 pos(source);
        while(world.isCollision(pos)==World::CollisionType::NOTHITTED && glm::distance(source, pos)<20){
            mint = 1e30;
            if (front.x != 0){
                x = (Fx(pos.x) - pos.x) / front.x;
                if(x < mint) mint = x;
            }
            if (front.y != 0){
                y = (Fy(pos.y) - pos.y) / front.y;
                if (y < mint) mint = y;
            }
            if (front.z != 0){
                z = (Fz(pos.z) - pos.z) / front.z;
                if (z < mint) mint = z;
            }
            pos.x += mint * front.x;
            pos.y += mint * front.y;
            pos.z += mint * front.z;
            pos += epsVec;
            //std::cout<<pos.x <<" "<<pos.y<<" "<<pos.z<<" "<<mint<<" "<<std::endl;
            //std::cout<<front.x <<" "<<front.y<<" "<<front.z<<std::endl;
        }
        hitPos = glm::ivec3(pos);
        return world.isCollision(pos)==World::CollisionType::HITTED;
    }
};
#endif