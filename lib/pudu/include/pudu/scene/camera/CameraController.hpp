#pragma once

namespace pudu
{

class Camera;
class Window;

class CameraController
{
public:
    CameraController(Camera& camera, Window& window) 
        : m_camera(camera), m_window(window) {}
    virtual ~CameraController() = default;

    virtual void updateCamera(float deltaTime) = 0;

    virtual const Camera& getCamera() const
    {
        return m_camera;
    }

protected:
    Camera& m_camera;
    Window& m_window;
};

}