#pragma once
#include <string>
#define GLM_FORCE_PURE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>

class Camera {
    public:
    Camera(float left, float right, float bottom, float top, float nearZ = 3.1f, float farZ = -100.0f)
     : m_position(0.f, 0.f, 3.f), m_scale(1.f, 1.f, 1.f), m_rotations(0.f, 0.f, 0.f)
    {
        m_projMat = glm::ortho(left, right, bottom, top, nearZ, farZ);
    }

    ~Camera() = default;

    inline const glm::mat4& getProjectionMatrix() const {
        return m_projMat;
    }

    inline void setProjectionMatrix(glm::mat4& newProjectionMatrix) {
        m_projMat = newProjectionMatrix;
    }

    inline glm::mat4 getViewMatrix() const {
        return glm::inverse(getModelMatrix());
    }

    inline glm::vec3 getScale() const {
        return m_scale;
    }

    inline glm::mat4 getModelMatrix() const {

        glm::mat4 translation = glm::translate(m_position);
        glm::mat4 rotationX = glm::rotate(m_rotations.x, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotationY = glm::rotate(m_rotations.y, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotationZ = glm::rotate(m_rotations.z, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 rotation = rotationZ * rotationY * rotationX;
        glm::mat4 scaling = glm::scale(m_scale);

        return translation * rotation * scaling;
    }
    
    void move(const glm::vec2& moveAmount) {
        m_position.x += moveAmount.x;
        m_position.y += moveAmount.y;
    }

    glm::vec2 getPosition() const {
        return glm::vec2(m_position.x, m_position.y);
    }

    inline void setPosition(glm::vec2 newPos) {
        m_position.x = newPos.x;
        m_position.y = newPos.y;
    }

    inline void setPosition(float newX, float newY) {
        m_position.x = newX;
        m_position.y = newY;
    }

    inline void setPosition(float newX, float newY, float newZ) {
        m_position.x = newX;
        m_position.y = newY;
        m_position.z = newZ;
    }

    inline void setPosition(glm::vec3 newPos) {
        m_position.x = newPos.x;
        m_position.y = newPos.y;
        m_position.z = newPos.z;
    }

    inline void setScale(glm::vec2 newScale) {
        m_scale.x = newScale.x;
        m_scale.y = newScale.y;
    }

    inline void setScale(float newScale) {
        m_scale.x = newScale;
        m_scale.y = newScale;
    }

    private:
    glm::mat4 m_projMat;

    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_rotations;
};