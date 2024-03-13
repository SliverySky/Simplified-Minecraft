#ifndef CAMER_H
#define CAMER_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SFML/Graphics.hpp>
enum CameraMovement{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};
const float YAW = 0.0f;
const float PITCH = 0.0f;
const float SPEED = 4.0f;
const float SENSITIVITY = 0.2f;
const float ZOOM = 45.0;

class Camera{
public:
    glm::vec3 position;
    glm::vec3 front, up, left;
    float pitch, yaw;
    float moveSpeed;
    float mouseSensitivity;
    float zoom;
    sf::RenderWindow *window=NULL;
    sf::Vector2i lastMousePosition;

    Camera(glm::vec3 _position = glm::vec3(0,0,0), float _pitch = PITCH, float _yaw = YAW):moveSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM){
        position = _position;
        pitch = _pitch;
        yaw = _yaw;
        lastMousePosition = sf::Mouse::getPosition();
        updateDirectionVectoers();
    }

    glm::mat4 getViewMatrix(){
        return glm::lookAt(position, position+front, up);
    }   

    void handEvent(sf::Event event, float deltaTime){
        processKeyboard(deltaTime);
        processMouse();
        updateDirectionVectoers();
    }
    void setBound(sf::RenderWindow *_window){
        window = _window;
    }


private:
    void processKeyboard(float deltaTime){
        float _moveSpeed = moveSpeed * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){ position += _moveSpeed * front; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){ position -= _moveSpeed * front; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)){ position += _moveSpeed * left; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)){ position -= _moveSpeed * left; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)){ position += _moveSpeed * up; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)){ position -= _moveSpeed * up; }
    }



    void processMouse(){
        sf::Vector2i _mousePosition = sf::Mouse::getPosition();
        float xoffset = mouseSensitivity * (_mousePosition.x - lastMousePosition.x);
        float yoffset = mouseSensitivity * (_mousePosition.y - lastMousePosition.y);
        
        yaw -= xoffset;
        pitch -= yoffset;

        //如果鼠标超出屏幕，重置鼠标
        // 获取窗口的全局边界
        

        // 判断鼠标是否超出窗口范围
        // if (window != NULL){
        //     sf::Vector2u windowSize = window->getSize();
        //     sf::Vector2i pos = sf::Mouse::getPosition(*window);
        //     if(pos.x < 0 || pos.x > windowSize.x){
        //             sf::Mouse::setPosition(sf::Vector2i(windowSize.x / 2, pos.y), *window);
        //      }
        // }
       // _mousePosition = sf::Mouse::getPosition();
        lastMousePosition.x = _mousePosition.x;
        lastMousePosition.y = _mousePosition.y;
    }
    void updateDirectionVectoers(){
        if (pitch > 360) pitch -= 360;
        if (yaw > 360) yaw -= 360;
        front.y = sin(glm::radians(pitch));
        front.x = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        front.z = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        
        up.y = sin(glm::radians(pitch+90));
        up.x = cos(glm::radians(pitch+90)) * sin(glm::radians(yaw));
        up.z = cos(glm::radians(pitch+90)) * cos(glm::radians(yaw));
        left = glm::cross(up,front);
    }

};

#endif