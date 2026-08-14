#pragma once

#include "Types.hpp"

#include <glm/glm.hpp>

#include <array>
#include <string>

class GLFWwindow;

namespace pudu
{

constexpr size_t MAX_KEYBOARD_KEY = 512;

class Window
{
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    bool isRunning() const;
    void closeWindow();

    void swapBuffers();
    void processEvents();

    glm::vec2 getSize() const;
    GLFWwindow* getGLFWWindow();

    double getTime() const;

    void setTitle(const std::string& title);
    inline std::string getTitle() const { return m_title; };

    void destroy();

    bool isMouseLeftButtonPressed() const;
    bool isMouseMiddleButtonPressed() const;
    bool isMouseRightButtonPressed() const;

    bool isKeyPressed(Key key) const;
    bool isKeyJustPressed(Key key) const;
    glm::vec2 getMousePosition() const;
    void setCursorEnabled(bool on);

    inline int getFPS() const { return 1.0f / m_deltaTime; }
    inline float getDeltaTime() const { return m_deltaTime; }
    inline float getDeltaTimeMs() const { return m_deltaTime * 1000.0f; }

private:
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

private:
    GLFWwindow* m_glfwWindow = nullptr;

    static std::array<bool, MAX_KEYBOARD_KEY> s_CurrentKeys;
    static std::array<bool, MAX_KEYBOARD_KEY> s_PreviousKeys;

    int m_width;
    int m_height;
    std::string m_title;

    // Delta time is stored in seconds
    float m_deltaTime = 0.0f;
    float m_lastFrame = 0.0f;
}; 

};
