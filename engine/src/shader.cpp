#include "shader.h"

Shader::Shader(const char* const vsString, const char* const fsString)
{
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vsString, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("VertexShaderCompile failed: \"%s\n", infoLog);
    }

    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fsString, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("FragmentShaderCompileFailed: \"%s\n", infoLog);
    }

    m_shaderProgram = glCreateProgram();

    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);

    if(!success) {
        glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
        printf("ShaderCompilation failed: \"%s\n", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
    glDeleteProgram(m_shaderProgram);
}

GLuint Shader::compileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = 0;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char info[512];
        glGetShaderInfoLog(shader, 512, nullptr, info);
        printf("Shader error: %s\n", info);
    }

    return shader;
}

void Shader::setUniform1i(const std::string& name, int value) {
    GLint location = glGetUniformLocation(m_shaderProgram, name.c_str());
    if (location < 0) {
	    return; 
    }
    glUniform1i(location, value);
}

void Shader::setUniform3f(const std::string& name, float x, float y, float z) {
    GLint location = glGetUniformLocation(m_shaderProgram, name.c_str());
	if (location < 0) {
        return;
	}
    glUniform3f(location, x, y, z);
}

void Shader::setUniform3f(const std::string& name, const glm::vec3& vector) {
    GLint location = glGetUniformLocation(m_shaderProgram, name.c_str());
	if (location < 0) {
        return;
	}
    glUniform3f(location, vector.x, vector.y, vector.z);
}

void Shader::setUniform4f(const std::string& name, float x, float y, float z, float w) {
    GLint location = glGetUniformLocation(m_shaderProgram, name.c_str());
	if (location < 0) {
        return;
	}
    glUniform4f(location, x, y, z, w);
}

void Shader::setUniform4f(const std::string& name, const glm::vec4& vector) {
    GLint location = glGetUniformLocation(m_shaderProgram, name.c_str());
	if (location < 0) {
        return;
	}
    glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

void Shader::setUniformMat4(const std::string& name, const glm::mat4& mat) {
    GLint location = glGetUniformLocation(m_shaderProgram, name.c_str());
    if(location < 0) {
        return;
    }
    glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::bind()
{
    glUseProgram(m_shaderProgram);
}

