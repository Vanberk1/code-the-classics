#pragma once

#include "Texture.hpp"

#include <glm/glm.hpp>

#include <map>
#include <filesystem>

namespace pudu 
{

struct Character {
    Texture texture;
    glm::ivec2 size;     
    glm::ivec2 bearing;
    unsigned int advance;
};

class Font
{
public:
    Font(const std::filesystem::path& filepath, uint32_t fontSize = 18);

    uint32_t getFonzSize() const { return m_fontSize; }
    const Character& getCharacter(unsigned char c) const;

private:
    // TODO: Combine all glyphs textures into one 
    // std::unique_ptr<Texture> m_fontAtlasTexture;
    std::filesystem::path m_filepath;

    std::map<unsigned char, Character> m_characters;
    uint32_t m_fontSize = 18;

};

}