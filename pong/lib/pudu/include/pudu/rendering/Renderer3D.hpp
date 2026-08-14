#pragma once

#include "rendering/Renderer.hpp"

#include "core/Types.hpp"
#include "scene/Light.hpp"
#include "scene/Mesh.hpp"

#include <vector>

namespace pudu
{

class Camera;
class Model;
class Shader;
struct Transform;
struct Vertex;

class Renderer3D: public Renderer
{
public:
    Renderer3D(Window& window, Camera& camera);
    virtual ~Renderer3D();

    void setDirectionalLight(const DirectionalLight& light);
    void addPointLight(const PointLight& light);
    void addSpotLight(const SpotLight& light);

    inline size_t getPointLightsNum() const { return m_pointLights.size(); }
    inline size_t getSpotLightsNum() const { return m_spotLights.size(); }

    DirectionalLight& getDirectionalLight() { return m_directionalLight; }
    PointLight& getPointLight(size_t i) { return m_pointLights[i]; }
    SpotLight& getSpotLight(size_t i) { return m_spotLights[i]; }
    
    void drawMesh(
        const Mesh& mesh, 
        const Transform& transform,
        Color color = { 1, 1, 1, 1 }
    );

    void drawModel(
        const Model& model,
        Color color = { 1, 1, 1, 1 }
    );

    void drawModel(
        const Model& model, 
        const Transform& transform,
        Color color = { 1, 1, 1, 1 }
    );

    void drawLight(
        const glm::vec3& position,
        Color color = { 1, 1, 1, 1 }
    );

    void showDebugGui() const;

private:
    void updateLightUniforms(const Shader& shader);

private:
    // TODO: Make a better light management system
    DirectionalLight m_directionalLight;
    std::vector<PointLight> m_pointLights;
    std::vector<SpotLight> m_spotLights;

    Mesh m_lightMesh = Mesh::CreateCubeMesh();
};

//-----------------------------------------------------------------------------

}

//-----------------------------------------------------------------------------