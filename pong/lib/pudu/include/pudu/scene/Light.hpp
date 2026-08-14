#pragma once

#include "core/Types.hpp"

#include <glm/glm.hpp>

namespace pudu
{

struct DirectionalLight
{
    glm::vec3 direction = { 0.2f, -1.0f, -0.3f };
    Color ambient = { 0.5f, 0.5f, 0.5 };
    Color diffuse = { 0.8f, 0.8f, 0.8f };
    Color specular = { 0.2f, 0.2f, 0.2f };
};

struct PointLight
{
    glm::vec3 position = { 0.0f, 0.0f, 0.0f };
    Color ambient = { 0.05f, 0.05f, 0.05f };
    Color diffuse = { 0.8f, 0.8f, 0.8f };
    Color specular = { 1.0f, 1.0f, 1.0f };
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

struct SpotLight
{
    glm::vec3 position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 direction = { 0.0f, 0.0f, 0.0f };
    Color ambient = { 0.1f, 0.1f, 0.1f };
    Color diffuse = { 0.8f, 0.8f, 0.8f };
    Color specular = { 1.0f, 1.0f, 1.0f };
    float cutOff = glm::cos(glm::radians(12.5f));
    float outerCutOff = glm::cos(glm::radians(17.5f));
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

}