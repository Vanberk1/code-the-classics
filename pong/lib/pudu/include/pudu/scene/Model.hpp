#pragma once

#include "Mesh.hpp"

#include <filesystem>
#include <unordered_map>
#include <vector>


namespace pudu
{
    
class Mesh;
class Texture;

using TexturesCacheMap = std::unordered_map<std::string, std::shared_ptr<Texture>>;

class Model
{
public:
    Model(const std::filesystem::path& filepath);

    const std::vector<Mesh>& getMeshes() const;

private:
    void load(const std::filesystem::path& filepath);

private:
    std::filesystem::path m_directory = "";
    std::vector<Mesh> m_meshes;
    TexturesCacheMap m_texturesCache;
};

}


