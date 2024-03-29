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
    struct Plan{
        glm::vec3 n;
        float d;
        void SetVal(const glm::vec3& norm, const glm::vec3& p){
            n = norm;
            d = glm::dot(norm, p);
        }
        bool IsPositive(const glm::vec3& v){
            return glm::dot(v, n) >= d;
        }
    }plans[6];
    sf::Vector2i lastMousePosition;
    sf::RenderWindow &window;
    const float zFar = 200.0f;
    const float zNear = 0.1f;
    const float fovy = glm::radians(45.0f);
    const float aspect = 800.0f / 600.0f;
    glm::mat4 projMat = glm::perspective(fovy, aspect, zNear, zFar);

    float halfVSide = zFar * glm::tan(fovy / 2);
    float halfHSide = halfVSide * aspect;
    glm::mat4 viewMat;
    Camera(sf::RenderWindow & window, glm::vec3 _position = glm::vec3(0,0,0), float _pitch = PITCH, float _yaw = YAW):moveSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM), window(window){
        position = _position;
        pitch = _pitch;
        yaw = _yaw;
        lastMousePosition = sf::Mouse::getPosition(window);
        updateDirectionVectoers();
        setViewMatrix();
        CalFrustum();
        for(int i = 0; i < 6; i++){
            std::cout<<i<<":"<<plans[i].n.x <<" "<<plans[i].n.y << " " << plans[i].n.z << " "<<plans[i].d <<std::endl;
        }

    }
    void CalFrustum(){
        plans[0].SetVal(front, front * zNear + position); // Near
        plans[1].SetVal(-front, front * zFar + position); // Far
        glm::vec3 farP = zFar * front;
        plans[2].SetVal(glm::normalize(-glm::cross(up, farP + left * halfHSide)), position); // Left
        plans[3].SetVal(glm::normalize(glm::cross(up, farP - left * halfHSide)), position); // Right
        plans[4].SetVal(glm::normalize(glm::cross(left, farP + up * halfVSide)), position); // Top
        plans[5].SetVal(glm::normalize(-glm::cross(left, farP - up * halfVSide)), position); // Top
    }

    bool IsInFrustum(glm::vec3 p1, glm::vec3 p2){
        for (int i = 0; i < 6; i++){
            bool isOutside = true;
            for (int k = 0; k < 8; k++){
                glm::vec3 p;
                p.x = (k&1) ? p1.x : p2.x;
                p.y = (k&2) ? p1.y : p2.y;
                p.z = (k&4) ? p1.z : p2.z;
                //glm::vec3 t = viewMat * glm::vec4(p, 1.0);
                if (plans[i].IsPositive(p)){
                    isOutside = false;
                    break;
                }
            }
            if (isOutside) return false;
        }
        return true;
    }

    glm::vec4 GetMatRow(glm::mat4& m, int i){
        return glm::vec4(m[i][0], m[i][1], m[i][2], m[i][3]);
    }

    void setViewMatrix(){
        viewMat = glm::lookAt(position, position+front, up);
    }   
    glm::mat4 getViewMatrix(){
        return viewMat;
    }   

    void HandEvent(sf::Event event, float deltaTime){
        processKeyboard(deltaTime);
        processMouse();
        updateDirectionVectoers();
        CalFrustum();
        setViewMatrix();
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