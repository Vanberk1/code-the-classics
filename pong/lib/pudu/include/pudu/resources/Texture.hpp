#pragma once

#include <filesystem>

namespace pudu 
{

struct OpenGLTextureState
{
    uint32_t internalFormat; // format of texture object
    uint32_t imageFormat; // format of loaded image texture configuration
    uint32_t wrapS; // wrapping mode on S axis
    uint32_t wrapT; // wrapping mode on T axis
    uint32_t filterMin; // filtering mode if texture pixels < screen pixels
    uint32_t filterMax; // filtering mode if texture pixels > screen pixels
};

enum class TextureType
{
    Diffuse,
    Specular
};

class Texture
{
public:
    Texture() = default;
    Texture(
        const std::filesystem::path& filepath, 
        TextureType type = TextureType::Diffuse, 
        uint32_t rows = 1, 
        uint32_t columns = 1
    );
    Texture(
        uint32_t width,
        uint32_t height,
        uint32_t numChannels,
        unsigned char* textureData,
        TextureType type = TextureType::Diffuse, 
        uint32_t rows = 1, 
        uint32_t columns = 1
    );

    uint32_t id() const { return m_textureID; }

    void generate(uint32_t width, uint32_t height, uint32_t numChannels, unsigned char* textureData);
    
    void bind() const;

    TextureType getType() const;
    void setType(TextureType type);

    uint32_t getInternalFormat() const;
    void setInternalFormat(uint32_t value);

    uint32_t getImageFormat() const;
    void setImageFormat(uint32_t value);

    std::string getTypeString() const;

    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }
    uint32_t getRows() const { return m_rows; }
    uint32_t getColumns() const { return m_columns; }
    uint32_t getFrames() const { return m_frames; }

    // Setting the number of rows or columns will recalculate the variable 
    // m_frames by m_rows * m_columns. 
    // NOTE: Number of frames variable m_frames can't be modified by its own.
    void setRows(const uint32_t rows );
    void setColumns(const uint32_t columns );

private:
    uint32_t m_textureID;

    std::filesystem::path m_filepath;

    uint32_t m_width = 0;
    uint32_t m_height = 0;
    // A texture may contain multiple frames (sprite sheet) arranged in rows and columns.
    // Each frame can be addressed by an index in the range [0, rows * columns[
    // By default, the texture contains a single frame.
    uint32_t m_rows = 1;
    uint32_t m_columns = 1;
    uint32_t m_frames = 1;

    TextureType m_type = TextureType::Diffuse;

    // Default m_state values
    //   internalFormat = GL_RGBA
    //   imageFormat = GL_RGBA
    //   wrapS = GL_REPEAT
    //   wrapT = GL_REPEAT
    //   filterMin = GL_NEAREST
    //   filterMax = GL_NEAREST
    OpenGLTextureState m_state;
};

};