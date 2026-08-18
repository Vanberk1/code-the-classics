#pragma once

#include <filesystem>
#include <string>

namespace pudu::utils
{
    std::string ReadFile(const std::filesystem::path& filepath);

    int GetRandomInt(int from, int to);
    float GetRandomFloat(float from, float to);

} 