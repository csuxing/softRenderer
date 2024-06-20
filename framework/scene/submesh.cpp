#include "submesh.h"
namespace SG
{
    SubMesh::SubMesh(const std::string& name):
        m_meshName(name)
    {
    }

    std::type_index SubMesh::getType()
    {
        return typeid(SubMesh);
    }

    void SubMesh::draw(VkCommandBuffer& cmd)
    {
        VkBuffer vertexBuffer = m_vertexBuffers.begin()->second.getHandle();
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer, &offset);
        VkBuffer indexBuffer = m_indexBuffer->getHandle();
        vkCmdBindIndexBuffer(cmd, indexBuffer, 0, m_indexType);
        vkCmdDrawIndexed(cmd, m_vertexIndices, 1, 0, 0, 0);
    }
}
