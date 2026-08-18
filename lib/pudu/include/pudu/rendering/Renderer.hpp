#pragma once

#include "utils/Profiler.hpp"

#include <glm/glm.hpp>

#include <map>
#include <memory>

namespace pudu
{

class Camera;
class Shader;
class Window;

struct RendererStats
{
    uint32_t drawCalls = 0;
    uint32_t vertices = 0;
    uint32_t indices = 0;
    uint32_t triangles = 0;
    double frameTimeMs = 0.0;
};

enum class ShaderType
{
    Model,
    LightObject,
    TextureQuad,
    TextSDF
};

class Renderer
{
public:
    Renderer(Window& window, Camera& camera);
    virtual ~Renderer();

    void setCurrentCamera(Camera& camera);
    Camera& getCurrentCamera() const;

    void setWireframeMode(bool on);
    inline bool getWireframeMode() const { return m_wireframeMode; };

    virtual void clearScene(glm::vec4 color);
    virtual void drawScene();

    const RendererStats& getRenderStats() const { return m_renderStats; }
    
private:
    void createDefaultTexture();
    void initShaders();

protected:
    Window& m_window;
    Camera& m_currentCamera;

    bool m_wireframeMode = false;
    
    int m_maxTextureUnits = 0;
    uint32_t m_defaultTextureId = 0;
    
    std::map<ShaderType, std::unique_ptr<Shader>> m_shaders;
    
    RendererStats m_renderStats;
    Profiler m_profiler;
};

}