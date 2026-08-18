#pragma once

#include "scene/camera/CameraController.hpp"

#include <glm/vec2.hpp>

namespace pudu
{

class FlyCameraController : public CameraController
{
public:
    FlyCameraController(Camera& camera, Window& window);
    ~FlyCameraController() = default;

    void updateCamera(float deltaTime) override;

    void setCameraSpeed(float cameraSpeed);
    float getCameraSpeed() const;

    void setMouseSensitivity(float sensitivity);
    float getMouseSensitivity() const;


private:
    float m_cameraSpeed = 20.0f;
    float m_mouseSensitivity = 0.1f;
    glm::vec2 m_lastCursorPos = { 0, 0 };
    bool m_firstCursorMove = true;
};

}