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
    sf::Vector2i lastMousePosition;
    sf::RenderWindow &window;
    glm::mat4 projMat = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    Camera(sf::RenderWindow & window, glm::vec3 _position = glm::vec3(0,0,0), float _pitch = PITCH, float _yaw = YAW):moveSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM), window(window){
        position = _position;
        pitch = _pitch;
        yaw = _yaw;
        lastMousePosition = sf::Mouse::getPosition(window);
        updateDirectionVectoers();
    }

    glm::mat4 getViewMatrix(){
        return glm::lookAt(position, position+front, up);
    }   

    void HandEvent(sf::Event event, float deltaTime){
        processKeyboard(deltaTime);
        processMouse();
        updateDirectionVectoers();
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
        //sf::Vector2u windowSize = window.getSize();
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2u windowSize = window.getSize();
        //如果鼠标超出屏幕，重置鼠标
        if(mousePos.x < 20 || mousePos.x > windowSize.x - 20){
            sf::Vector2i resetPos = sf::Vector2i(windowSize.x / 2, mousePos.y);
            sf::Mouse::setPosition(resetPos, window);
            lastMousePosition = resetPos;
        }
        float xoffset = mouseSensitivity * (mousePos.x - lastMousePosition.x);
        float yoffset = mouseSensitivity * (mousePos.y - lastMousePosition.y);
        //std::cout<<lastMousePosition.x <<" "<<lastMousePosition.y<<std::endl;
        yaw -= xoffset;
        pitch -= yoffset;
        lastMousePosition = mousePos;
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