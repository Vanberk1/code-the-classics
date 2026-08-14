#pragma once

#include "Types.hpp"

#include <string_view>
#include <memory>

#include <glm/glm.hpp>

namespace pudu
{

class Camera;
class Font;
class Model;
class Renderer2D;
class Renderer3D;
class Texture;
class Window;

struct Color;
struct Transform;

class Application
{
public:
    Application(uint32_t width, uint32_t height, std::string_view title);
    ~Application();

    void close();
    bool isRunning() const;

    int getFPS() const;
    float getDeltaTime() const;
    float getDeltaTimeMs() const;

    bool isMouseLeftButtonPressed() const;
    bool isMouseMiddleButtonPressed() const;
    bool isMouseRightButtonPressed() const;

    bool isKeyPressed(Key key) const;
    bool isKeyJustPressed(Key key) const;
    glm::vec2 getMousePosition() const;
    void setCursorEnabled(bool on);

    void processInput();
    void clearFrame(Color color);
    void showFrame();

    /** 2D Drawing methods */
    void drawRect(
        glm::vec2 position, 
        glm::vec2 size, 
        float rotate = 0.0f, 
        glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        bool filled = true
    );
    void drawTexture(
        const Texture& texture, 
        glm::vec2 position, 
        glm::vec2 size, 
        float rotate = 0.0f, 
        glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        bool flipH = false,
        bool flipV = false
    );
    void drawTextureFrame(
        const Texture& texture, 
        glm::vec2 position, 
        glm::vec2 size,
        uint32_t frame,
        float rotate = 0.0f, 
        glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        bool flipH = false,
        bool flipV = false
    );
    void drawText(
        std::string_view text,
        const Font& font, 
        glm::vec2 position,
        glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
    );

    /** 3D Drawing methods */
    void drawModel(const Model& model);
    void drawModel(const Model& model, const Transform& transform);

private:
    Application(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer2D> m_renderer2D;
    std::unique_ptr<Renderer3D> m_renderer3D;
    std::shared_ptr<Camera> m_camera2D;
    std::shared_ptr<Camera> m_camera3D;
};

}