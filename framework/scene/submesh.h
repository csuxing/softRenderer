#ifndef _SUBMESH_H_H_
#define _SUBMESH_H_H_
#include <glm/glm.hpp>

#include <memory>
#include <unordered_map>
#include <string>

#include "core/buffer.h"
#include "component.h"
#include "basic_define.h"
namespace SG
{
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec4 joint0;
        glm::vec4 weight0;
        static inline VkVertexInputBindingDescription getVertexInputBindDesc()
        {
            VkVertexInputBindingDescription vertexInputBinding{
                0,
                sizeof(Vertex),
                VK_VERTEX_INPUT_RATE_VERTEX
            };
            return vertexInputBinding;
        }

        static inline std::vector<VkVertexInputAttributeDescription> getVertexInputAttributeDesc()
        {
            std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescs{
                {0,0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos)},
                {1,0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)},
                {2,0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)},
                {3,0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, joint0)},
                {4,0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, weight0)},
            };
            return vertexInputAttributeDescs;
        }
    };

    class SubMesh : public Component
    {
    public:
        SubMesh(const std::string& name = {});
        virtual std::type_index getType() override;
        void draw(VkCommandBuffer& cmd);
        std::uint32_t m_vertexIndices{};
        VkIndexType   m_indexType{};

        std::unordered_map<std::string, RHI::Buffer> m_vertexBuffers;
        std::unique_ptr<RHI::Buffer> m_indexBuffer;
    private:
        std::string m_meshName{};
    };
}
#endif
