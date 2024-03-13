#ifndef SKYRENDER_H
#define SKYRENDER_H
#include <vector>
#include <string>
#include <glad/glad.h>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <iostream>
#include "shader.h"
#include "camera.h"
class SkyRender{
public:
    Shader shader;
    unsigned int VAO, VBO;
    unsigned int skyTexture;
    SkyRender():shader("./shader/skybox.vs", "./shader/skybox.fs"){
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
        std::vector<std::string> faces{
            "./resources/textures/left.png",
            "./resources/textures/right.png",
            "./resources/textures/top.png",
            "./resources/textures/bottom.png",
            "./resources/textures/front.png",
            "./resources/textures/back.png"
        };
        skyTexture = loadCubemap(faces);
        shader.use();
        shader.setInt("skybox", 0);
    }
    void Render(glm::mat4 viewMat, glm::mat4 projMat){
        glDepthFunc(GL_LEQUAL);
        shader.use();
        shader.setMat4("view", glm::mat4(glm::mat3(viewMat)));
        shader.setMat4("projection", projMat);
        shader.setInt("skybox", 0);
        //std::cout <<"id="<< textureID << std::endl;
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexture);

        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
    }
    ~SkyRender(){
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }
private:
    
    float skyboxVertices[108] = {
            // +x
        1.0f,  1.0f,  1.0f, 
        1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
            // -x
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f, 
        // +y 
         1.0f, 1.0f,  1.0f,
         1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f,  1.0f,
         1.0f, 1.0f,  1.0f,
        // -y
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        // +z
         1.0f,  1.0f, 1.0f, 
        -1.0f,  1.0f, 1.0f, 
        -1.0f, -1.0f, 1.0f, 
        -1.0f, -1.0f, 1.0f, 
         1.0f, -1.0f, 1.0f, 
         1.0f,  1.0f, 1.0f, 
        // -z
         1.0f,  1.0f, -1.0f, 
        -1.0f,  1.0f, -1.0f, 
        -1.0f, -1.0f, -1.0f, 
        -1.0f, -1.0f, -1.0f, 
         1.0f, -1.0f, -1.0f, 
         1.0f,  1.0f, -1.0f
    };
// float skyboxVertices[108] = {
//     //+x
//          1.0f, -1.0f, -1.0f,
//          1.0f, -1.0f,  1.0f,
//          1.0f,  1.0f,  1.0f,
//          1.0f,  1.0f,  1.0f,
//          1.0f,  1.0f, -1.0f,
//          1.0f, -1.0f, -1.0f,
//                 -1.0f, -1.0f,  1.0f,
//         -1.0f, -1.0f, -1.0f,
//         -1.0f,  1.0f, -1.0f,
//         -1.0f,  1.0f, -1.0f,
//         -1.0f,  1.0f,  1.0f,
//         -1.0f, -1.0f,  1.0f,
//             -1.0f,  1.0f, -1.0f,
//          1.0f,  1.0f, -1.0f,
//          1.0f,  1.0f,  1.0f,
//          1.0f,  1.0f,  1.0f,
//         -1.0f,  1.0f,  1.0f,
//         -1.0f,  1.0f, -1.0f,

//         -1.0f, -1.0f, -1.0f,
//         -1.0f, -1.0f,  1.0f,
//          1.0f, -1.0f, -1.0f,
//          1.0f, -1.0f, -1.0f,
//         -1.0f, -1.0f,  1.0f,
//          1.0f, -1.0f,  1.0f
//                 -1.0f, -1.0f,  1.0f,
//         -1.0f,  1.0f,  1.0f,
//          1.0f,  1.0f,  1.0f,
//          1.0f,  1.0f,  1.0f,
//          1.0f, -1.0f,  1.0f,
//         -1.0f, -1.0f,  1.0f,
//         // positions          
//         -1.0f,  1.0f, -1.0f,
//         -1.0f, -1.0f, -1.0f,
//          1.0f, -1.0f, -1.0f,
//          1.0f, -1.0f, -1.0f,
//          1.0f,  1.0f, -1.0f,
//         -1.0f,  1.0f, -1.0f,








//      };
    unsigned int loadCubemap(std::vector<std::string> faces){
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nChannel;
        stbi_set_flip_vertically_on_load(true);
        for (unsigned int i = 0; i < faces.size(); i++){
            unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nChannel, 0);
            if(data){
                GLenum format;
                if (nChannel == 1){
                    format = GL_RED;
                }else if (nChannel == 3){
                    format = GL_RGB;
                }else if (nChannel == 4){
                    format = GL_RGBA;
                }
                //std::cout<<"Cube texture format:"<<nChannel<<std::endl;
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            }else{
                std::cout << "Cubemap texture failed to load at " << faces[i] << std::endl;
                
            }
            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        return textureID;
    }
};
#endif