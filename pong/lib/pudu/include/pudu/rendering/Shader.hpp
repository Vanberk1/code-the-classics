#pragma once

#include <glm/glm.hpp>

#include <filesystem>
#include <string>

namespace pudu
{

class Shader
{
public:
    Shader(
        const std::filesystem::path& vertexPath, 
        const std::filesystem::path& fragmentPath);
    Shader(
        std::string_view vertexSource, 
        std::string_view fragmentSource);
    ~Shader();

    uint32_t id() const { return m_programID; }

    void bind() const;
    void unbind() const;

    void setUniformBool(const std::string& name, bool value) const;
    void setUniformInt(const std::string& name, int value) const;
    void setUniformFloat(const std::string& name, float value) const;
    void setUniformVec2(const std::string& name, const glm::vec2& vector) const;
    void setUniformVec3(const std::string& name, const glm::vec3& vector) const;
    void setUniformVec4(const std::string& name, const glm::vec4& vector) const;
    void setUniformMat3(const std::string& name, const glm::mat3& matrix) const;
    void setUniformMat4(const std::string& name, const glm::mat4& matrix) const;

private:
    void compileShader(
        std::string_view vertexSource, 
        std::string_view fragmentSource
    );

private:
    uint32_t m_programID;
};

};