#ifndef _SUBMESH_H_H_
#define _SUBMESH_H_H_
#include <glm/glm.hpp>

#include <memory>
#include <unordered_map>
#include <string>

#include "core/buffer.h"

#include "component.h"
namespace Scene
{
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec4 joint0;
        glm::vec4 weight0;
    };

    class SubMesh : public Component
    {
    public:
        SubMesh(const std::string& name = {});
        std::uint32_t m_vertexIndices{};
        VkIndexType   m_indexType{};

        std::unordered_map<std::string, RHI::Buffer> m_vertexBuffers;
        std::unique_ptr<RHI::Buffer> m_indexBuffer;
    private:
        std::string m_meshName{};
    };
}
#endif
