#pragma once

#include <glm/glm.hpp>

#include <array>
#include <memory>
#include <vector>

namespace pudu
{

class Texture;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Face
{
    std::array<uint32_t, 3> indices;
};

struct Transform
{
    glm::vec3 position = { 0, 0, 0 };
    glm::vec3 rotation = { 0, 0, 0 };
    glm::vec3 scale = { 1, 1, 1 };
};

class Mesh
{
public:
    Mesh(const std::vector<Vertex>& vertices, 
         const std::vector<Face>& faces, 
         const std::vector<std::shared_ptr<Texture>>& textures
    );
    // TODO: Move construstors and destructor are user-defined because Mesh class manages the OpenGL buffer
    // handles. This should be abstracted to a graphics platform layer and Mesh class 
    // should be just a data container with default destructor.
    Mesh(Mesh&&) noexcept;
    Mesh& operator=(Mesh&&) noexcept;
    ~Mesh();

    uint32_t vao() const { return m_vao; }
    uint32_t vbo() const { return m_vbo; }
    uint32_t ebo() const { return m_ebo; }

    uint32_t getNumOfVertices() const { return m_vertices.size(); }
    uint32_t getNumOfFaces() const { return m_faces.size(); }
    uint32_t getNumOfIndices() const { return m_faces.size() * 3; } // Right now all faces are triangles

    const std::vector<Vertex>& getVertices() const;
    void setVertices(const std::vector<Vertex>& vertices);
    
    const std::vector<Face>& getFaces() const;
    void setFaces(const std::vector<Face>& faces);
    
    const std::vector<std::shared_ptr<Texture>>& getTextures() const;
    void setTextures(const std::vector<std::shared_ptr<Texture>>& textures);

    static Mesh CreateCubeMesh();

private:
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    
    void initMesh();

    void debugPrint() const;

private:
    // TODO: Abstract graphics API buffer objects.
    uint32_t m_vao = 0;
    uint32_t m_vbo = 0;
    uint32_t m_ebo = 0;

    std::vector<Vertex> m_vertices = {};
    std::vector<Face> m_faces = {};
    std::vector<std::shared_ptr<Texture>> m_textures = {};
};

}