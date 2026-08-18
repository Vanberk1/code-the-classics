#pragma once

#include <glm/glm.hpp>

namespace pudu
{

class Camera
{
public:
    Camera() = default;
    Camera(
        const glm::vec3& position, const glm::vec3& up = glm::vec3(0, 1, 0));

    void setPosition(const glm::vec3& position);
    glm::vec3 getPosition() const;

    void lookAt(const glm::vec3& direction);

    void setOrthographic(glm::vec2 size, float near, float far);
    void setPerspective(glm::vec2 size, float fov, float near, float far);

    glm::vec3 getDirection() const;
    glm::vec3 getFront() const;
    glm::vec3 getRight() const;
    glm::vec3 getUp() const;
    glm::vec3 getWorldUp() const;
    glm::mat4 getViewMatrix() const;
    const glm::mat4& getProjectionMatrix() const;

    float getYaw() const;
    float getPitch() const;
    
    void updateCameraDirection(
        const glm::vec2& position, float sensitivity, bool constrainPitch = true);

private:
    void updateCameraVectors();

private:
    glm::vec3 m_position = glm::vec3(0, 0, 0);
    glm::vec3 m_frontVector = glm::vec3(0, 0, -1);
    glm::vec3 m_upVector = glm::vec3(0, 1, 0);
    glm::vec3 m_worldUpVector = glm::vec3(0, 1, 0);
    glm::vec3 m_rightVector = glm::vec3(1, 0, 0);
    float m_yaw = -90.0f;
    float m_pitch = 0.0f;
    float m_zoom = 1.0f;

    glm::mat4 m_projectionMatrix = glm::mat4(1.0);
};

}