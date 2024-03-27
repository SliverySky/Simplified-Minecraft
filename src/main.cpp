#include <glad/glad.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include "shader.h"
#include "camera.h"
#include "world.h"
#include "skyRender.h"
#include "BuildTool.h"
void renderQuad();
void print(glm::vec4 v, std::string name){
    std::cout<<name<<":"<<v.x <<" "<<v.y<<" "<<v.z<<" "<<v.w<<std::endl;
}
bool stop = false;
int main(){
    const int srcWidth = 800;
    const int srcHeight = 600;
    //很重要，SFML默认没有深度buffer
    sf::ContextSettings window_settings;
    window_settings.depthBits         = 24; // Request a 24-bit depth buffer
    window_settings.stencilBits       = 8;  // Request a 8 bits stencil buffer
    window_settings.antialiasingLevel = 2;  // Request 2 levels of antialiasing

    sf::RenderWindow window(sf::VideoMode(800, 600), "MineCraft",sf::Style::Resize | sf::Style::Close, window_settings);
    
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(sf::Context::getFunction))){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // 将鼠标的初始位置设置为窗口中心
    sf::Vector2u windowSize = window.getSize();
    sf::Mouse::setPosition(sf::Vector2i(windowSize.x / 2, windowSize.y / 2), window);
    window.setMouseCursorVisible(false);
    //glEnable(GL_TEXTURE_2D);
    // glEnable(GL_BLEND); 
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 设置混合函数
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //window.setVerticalSyncEnabled(true);

    glViewport(0, 0, srcWidth, srcHeight);

    Camera camera(window, glm::vec3(100, 30, 100));

    bool running = true;
    sf::Clock clock;
    int cnt = 0;
    glm::mat4 modelMat = glm::mat4(1.0f);
    Shader nowShader("./shader/box.vs", "./shader/box.fs");
    // important

    //渲染阴影
    const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    //避免绘制和读取数据
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Shader depthShader("./shader/depth.vs", "./shader/depth.fs");
    Shader debugShader("./shader/debug.vs", "./shader/debug.fs");
    
    SkyRender skyRender;
    
    std::ifstream file("./settings.txt");
    std::string line;
    
    int x, y, z;
    bool isDebug;
    float nearPlane = 1.0f, farPlane = 300.0f;
    float width = 100.0f;
    std::getline(file, line);
    std::istringstream iss = std::istringstream(line);
    iss >> x >> y >> z >>isDebug;
    std::getline(file, line);
    iss = std::istringstream(line);
    iss >> width >>nearPlane>>farPlane;
    glm::vec3 lightPos(x, y, z);

    World world(lightPos);
    nowShader.use();
    nowShader.setInt("texture_diffuse", 0);
    nowShader.setInt("shadowMap", 1);
    debugShader.use();
    debugShader.setInt("depthMap", 0);
    BuildTool buildTool;
    window.setMouseCursorGrabbed(true);
    while(running){
        sf::Event event;
        while(window.pollEvent(event)){
            if (event.type == sf::Event::Closed){
                running = false;
            }else if(event.type == sf::Event::Resized){
                glViewport(0, 0, event.size.width, event.size.height);
            }else if(event.type == sf::Event::GainedFocus){
                stop = false;
            }else if (event.type == sf::Event::LostFocus){
                stop = true;
            }else if (event.type == sf::Event::KeyPressed){
                if (event.key.code == sf::Keyboard::Escape){
                    running = false;
                }
            }
        }    
        if (stop) continue; 
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 渲染深度纹理
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        
        lightProjection = glm::ortho(-width, width, -width, width, nearPlane, farPlane);
        lightView = glm::lookAt(lightPos, glm::vec3(0, 0, 0), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        glm::vec4 tpos = lightSpaceMatrix * glm::vec4(10, 2, 10, 1);
        //std::cout<<tpos.x << " "<< tpos.y << " " << tpos.z <<std::endl;
        depthShader.use();
        depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        depthShader.setMat4("model", glm::mat4(1.0f));
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        world.Render(depthShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //重置屏幕
        glViewport(0, 0, srcWidth, srcHeight);
        //glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        


    
    if (isDebug){
        // 用于可视化debugging
        // ---------------------------------------------
        debugShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        renderQuad();
    }else{
        //渲染世界
        sf::Vector2u windowSize = window.getSize();
        glm::mat4 projMat = camera.projMat;
        glm::mat4 viewMat = camera.getViewMatrix();
       //glm::mat4 projMat = lightProjection;
       // glm::mat4 viewMat = lightView;
        glm::mat4 modelMat = glm::mat4(1.0f);

        glm::vec4 t1 = projMat * viewMat * modelMat * glm::vec4(0,0,0,1);
        glm::vec4 t2 = projMat * viewMat * modelMat * glm::vec4(0,0,1,1);
        nowShader.use();
        nowShader.setMat4("projection", projMat);
        nowShader.setMat4("view", viewMat);
        nowShader.setMat4("model", modelMat);
        nowShader.setVec3("viewPos", camera.position);
        nowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        world.Render(nowShader);

        // //渲染天空盒子

        skyRender.Render(viewMat, projMat);

        sf::Time dt = clock.restart();
        camera.HandEvent(event, dt.asSeconds());
        buildTool.HandEvent(event, dt.asSeconds());
        buildTool.Work(window, camera, world, dt.asSeconds());
    }
           window.display();

    }
}
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
