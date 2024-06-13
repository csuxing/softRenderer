#define TINYGLTF_IMPLEMENTATION // otherwise no link
#include "gltf_loader.h"

#include <glm/gtc/type_ptr.hpp>
#include "glm/glm.hpp"

#include "macro.h"

#include "core/buffer.h"
namespace Scene
{
    GltfLoader::GltfLoader(APP::VkDeviceManager* deviceManager):
        m_deviceManager(deviceManager)
    {

    }
    std::unique_ptr<SubMesh> GltfLoader::read_model_from_file(const std::string& file_name, uint32_t index)
    {
        std::string err;
        std::string warn;

        tinygltf::TinyGLTF gltf_loader;

        bool importResult = gltf_loader.LoadASCIIFromFile(&m_model, &err, &warn, file_name.c_str());

        if (!importResult)
        {
            LOG_ERROR("Failed to load gltf file {}", file_name.c_str());
        }

        if (!err.empty())
        {
            LOG_ERROR("error loading gltfModel : {}", err.c_str());
        }

        if (!warn.empty())
        {
            LOG_INFO("", warn.c_str());
        }

        return std::move(loadModel(index));
    }

    std::unique_ptr<SubMesh> GltfLoader::loadModel(uint32_t index)
    {
        auto submesh = std::make_unique<SubMesh>();
        auto& gltfMesh = m_model.meshes.at(index);
        auto& gltfPrimitive = gltfMesh.primitives.at(0);

        std::vector<Vertex> vertexData;

        const float* pos = nullptr;
        const float* normals = nullptr;
        const float* uvs = nullptr;
        const uint16_t* joints = nullptr;
        const float* weights = nullptr;

        auto& accessor = m_model.accessors[gltfPrimitive.attributes.find("POSITION")->second];
        size_t vertexCount = accessor.count;
        auto& bufferView = m_model.bufferViews[accessor.bufferView];
        pos = reinterpret_cast<const float*>(&(m_model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]));

        if (gltfPrimitive.attributes.find("Normal") != gltfPrimitive.attributes.end())
        {
            accessor = m_model.accessors[gltfPrimitive.attributes.find("NORMAL")->second];
            bufferView = m_model.bufferViews[accessor.bufferView];
            normals = reinterpret_cast<const float*>(&(m_model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]));
        }

        if (gltfPrimitive.attributes.find("TEXCOORD_0") != gltfPrimitive.attributes.end())
        {
            accessor = m_model.accessors[gltfPrimitive.attributes.find("TEXCOORD_0")->second];
            bufferView = m_model.bufferViews[accessor.bufferView];
            uvs = reinterpret_cast<const float*>(&(m_model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]));
        }

        for (size_t v = 0; v < vertexCount; ++v)
        {
            Vertex vert{};
            vert.pos = glm::vec4(glm::make_vec3(&pos[v * 3]), 1.0f);
            vert.normal = glm::normalize(glm::vec3(normals ? glm::make_vec3(&normals[v * 3]) : glm::vec3(0.0f)));
            vert.uv = uvs ? glm::make_vec2(&uvs[v * 2]) : glm::vec2(0.0f);
            vertexData.push_back(vert);
        }

        RHI::Buffer stageBuffer{ m_deviceManager, vertexData.size() * sizeof(vertexData),
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY };

        stageBuffer.update(vertexData.data(), vertexData.size() * sizeof(vertexData));

        RHI::Buffer buffer{ m_deviceManager, vertexData.size() * sizeof(vertexData),
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY };
        // todo : commandpool && commandbuffer, copy buffer
        
        // todo : store vkbuffer to submesh, to draw

        return std::unique_ptr<SubMesh>();
    }
}