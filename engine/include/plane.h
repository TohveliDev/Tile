#pragma once
//emscripten&GLFW&GLES
#include <GLES3/gl3.h>
//glm
#define GLM_FORCE_PURE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>
//std
#include <string>
//omat
#include "shader.h"
#include "camera.h"


//fwd declaration
class Shader;
class Camera;

class Plane {
    public:
        Plane(float xPos = 0.0f, float yPos = 0.0f, float width = 1.0f, float height = 1.0f);
        Plane(glm::vec2 pos = {0.0f, 0.0f}, glm::vec2 size = {1.0f, 1.0f});
        ~Plane();

        void render(Shader* shader, Camera* camera, GLuint texId = 0);

        void setScale(glm::vec2 newScale);
        void setScale(float newScale);
        void setScale(float newX, float newY);

        void setPosition(const glm::vec2 pos);
        void setPosition(float x, float y);

        void setColor(float r, float g, float b, float a = 1.0f);
        void setColor(glm::vec4 newColor);
        void setColor(glm::vec3 newColor);

        void setTexture(GLuint tex);

        GLuint getTextureID() { return m_texID; }
        glm::mat4 getModelMatrix() const;

    private:
        void initPlane(glm::vec2 pos = {0.0f, 0.0f}, glm::vec2 size = {1.0f, 1.0f});
        GLuint m_positionsVbo = 0;
        GLuint m_vao = 0;
        GLuint m_texID = 0;
        float color[4]; //Color as rgba
        GLuint m_texCoordsVbo = 0;
        glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
        glm::vec3 m_scale = {1.0f, 1.0f, 1.0f};
};