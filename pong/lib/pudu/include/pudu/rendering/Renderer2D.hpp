#pragma once

#include "rendering/Renderer.hpp"

#include <string_view>

namespace pudu
{

class Font;
class Texture;
class Shader;
class Window;
class Scene2D;

class Renderer2D: public Renderer
{
public:
    Renderer2D(Window& window, Camera& camera);
    virtual ~Renderer2D();
    
    void drawRect(
        glm::vec2 position, 
        glm::vec2 size, 
        float rotate = 0.0f, 
        glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        bool filled = true
    );
    void drawRect(
        const Shader& shader,
        glm::vec2 position, 
        glm::vec2 size, 
        float rotate = 0.0f, 
        glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        bool filled = true
    );

    // TODO: Maybe crate a TextureProperties struct to pass parameters
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

private:
    void createRectVertexData();

    uint32_t m_rectVAO;
    uint32_t m_rectVBO;
    uint32_t m_texCoordsVBO;
    uint32_t m_rectEBO;
};

}