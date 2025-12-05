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

class Shader
{
public:
    Shader(const char* const vsString, const char* const fsString);
    ~Shader();

    GLuint compileShader(GLenum type, const char* source);
    void bind();
    
    //Uniform setters: 
    void setUniform1i(const std::string& name, int value);
    void setUniform3f(const std::string& name, float x, float y, float z);
    void setUniform3f(const std::string& name, const glm::vec3& vector);
    void setUniform4f(const std::string& name, float x, float y, float z, float w);
    void setUniform4f(const std::string& name, const glm::vec4& vector);
    void setUniformMat4(const std::string& name, const glm::mat4& mat);


    GLint getID() { return m_shaderProgram; }
    
private:
    GLint m_shaderProgram;
};