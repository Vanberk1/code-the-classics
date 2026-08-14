#pragma once

#include <filesystem>
#include <unordered_map>
#include <string>

namespace pudu
{

class Font;
class Model;
class Shader;
class Texture;

class ResourceManager
{
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    std::shared_ptr<Font> loadFont(
        const std::string& name,
        uint32_t fontSize,
        const std::filesystem::path& filepath);
    std::shared_ptr<Model> loadModel(
        const std::string& name, 
        const std::filesystem::path& filepath);
    std::shared_ptr<Shader> loadShader(
        const std::string& name, 
        const std::filesystem::path& vertexPath, 
        const std::filesystem::path& fragmentPath);
    std::shared_ptr<Shader> loadShader(
        const std::string& name, 
        std::string_view vertexSource, 
        std::string_view fragmentSource);
    std::shared_ptr<Texture> loadTexture(
        const std::string& name, 
        const std::filesystem::path& filepath);

    std::shared_ptr<Font> getFont(const std::string& name);
    std::shared_ptr<Model> getModel(const std::string& name);
    std::shared_ptr<Shader> getShader(const std::string& name);
    std::shared_ptr<Texture> getTexture(const std::string& name);

private:
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    
private:
    std::unordered_map<std::string, std::shared_ptr<Font>> m_fonts;
    std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;

};

}