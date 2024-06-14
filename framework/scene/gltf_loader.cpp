#define TINYGLTF_IMPLEMENTATION // otherwise no link
#include "gltf_loader.h"

#include <glm/gtc/type_ptr.hpp>
#include "glm/glm.hpp"

#include "macro.h"

#include "core/buffer.h"
#include "core/command_buffer.h"

#include "submesh.h"
#include "core/helpers.h"

namespace Scene
{
    inline size_t getAttributeSize(const tinygltf::Model* model, uint32_t accessorId)
    {
        return model->accessors.at(accessorId).count;
    }
    inline VkFormat getAttributeFormat(const tinygltf::Model* model, uint32_t accessorId)
    {
        auto& accessor = model->accessors.at(accessorId);
        VkFormat format;

        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_BYTE:
        {
            static const std::map<int, VkFormat> mappedFormat = { {TINYGLTF_TYPE_SCALAR, VK_FORMAT_R8_SINT},
                                                                  {TINYGLTF_TYPE_VEC2, VK_FORMAT_R8G8_SINT},
                                                                  {TINYGLTF_TYPE_VEC3, VK_FORMAT_R8G8B8_SINT},
                                                                  {TINYGLTF_TYPE_VEC4, VK_FORMAT_R8G8B8A8_SINT} };
            format = mappedFormat.at(accessor.type);
            break;
        }
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
            static const std::map<int, VkFormat> mappedFormat = { {TINYGLTF_TYPE_SCALAR, VK_FORMAT_R8_UINT},
                                                                  {TINYGLTF_TYPE_VEC2, VK_FORMAT_R8G8_UINT},
                                                                  {TINYGLTF_TYPE_VEC3, VK_FORMAT_R8G8B8_UINT},
                                                                  {TINYGLTF_TYPE_VEC4, VK_FORMAT_R8G8B8A8_UINT} };

            static const std::map<int, VkFormat> mappedFormatNormalize = { {TINYGLTF_TYPE_SCALAR, VK_FORMAT_R8_UNORM},
                                                                            {TINYGLTF_TYPE_VEC2, VK_FORMAT_R8G8_UNORM},
                                                                            {TINYGLTF_TYPE_VEC3, VK_FORMAT_R8G8B8_UNORM},
                                                                            {TINYGLTF_TYPE_VEC4, VK_FORMAT_R8G8B8A8_UNORM} };

            if (accessor.normalized)
            {
                format = mappedFormatNormalize.at(accessor.type);
            }
            else
            {
                format = mappedFormat.at(accessor.type);
            }

            break;
        }
        case TINYGLTF_COMPONENT_TYPE_SHORT: {
            static const std::map<int, VkFormat> mapped_format = { {TINYGLTF_TYPE_SCALAR, VK_FORMAT_R8_SINT},
                                                                  {TINYGLTF_TYPE_VEC2, VK_FORMAT_R8G8_SINT},
                                                                  {TINYGLTF_TYPE_VEC3, VK_FORMAT_R8G8B8_SINT},
                                                                  {TINYGLTF_TYPE_VEC4, VK_FORMAT_R8G8B8A8_SINT} };

            format = mapped_format.at(accessor.type);

            break;
        }
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
            static const std::map<int, VkFormat> mapped_format = { {TINYGLTF_TYPE_SCALAR, VK_FORMAT_R16_UINT},
                                                                  {TINYGLTF_TYPE_VEC2, VK_FORMAT_R16G16_UINT},
                                                                  {TINYGLTF_TYPE_VEC3, VK_FORMAT_R16G16B16_UINT},
                                                                  {TINYGLTF_TYPE_VEC4, VK_FORMAT_R16G16B16A16_UINT} };

            static const std::map<int, VkFormat> mapped_format_normalize = { {TINYGLTF_TYPE_SCALAR, VK_FORMAT_R16_UNORM},
                                                                            {TINYGLTF_TYPE_VEC2, VK_FORMAT_R16G16_UNORM},
                                                                            {TINYGLTF_TYPE_VEC3, VK_FORMAT_R16G16B16_UNORM},
                                                                            {TINYGLTF_TYPE_VEC4, VK_FORMAT_R16G16B16A16_UNORM} };

            if (accessor.normalized)
            {
                format = mapped_format_normalize.at(accessor.type);
            }
            else
            {
                format = mapped_format.at(accessor.type);
            }

            break;
        }
        case TINYGLTF_COMPONENT_TYPE_INT: {
            static const std::map<int, VkFormat> mapped_format = { {TINYGLTF_TYPE_SCALAR, VK_FORMAT_R32_SINT},
                                                                  {TINYGLTF_TYPE_VEC2, VK_FORMAT_R32G32_SINT},
                                                                  {TINYGLTF_TYPE_VEC3, VK_FORMAT_R32G32B32_SINT},
                                                                  {TINYGLTF_TYPE_VEC4, VK_FORMAT_R32G32B32A32_SINT} };

            format = mapped_format.at(accessor.type);

            break;
        }
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
            static const std::map<int, VkFormat> mapped_format = { {TINYGLTF_TYPE_SCALAR, VK_FORMAT_R32_UINT},
                                                                  {TINYGLTF_TYPE_VEC2, VK_FORMAT_R32G32_UINT},
                                                                  {TINYGLTF_TYPE_VEC3, VK_FORMAT_R32G32B32_UINT},
                                                                  {TINYGLTF_TYPE_VEC4, VK_FORMAT_R32G32B32A32_UINT} };

            format = mapped_format.at(accessor.type);

            break;
        }
        case TINYGLTF_COMPONENT_TYPE_FLOAT: {
            static const std::map<int, VkFormat> mapped_format = { {TINYGLTF_TYPE_SCALAR, VK_FORMAT_R32_SFLOAT},
                                                                  {TINYGLTF_TYPE_VEC2, VK_FORMAT_R32G32_SFLOAT},
                                                                  {TINYGLTF_TYPE_VEC3, VK_FORMAT_R32G32B32_SFLOAT},
                                                                  {TINYGLTF_TYPE_VEC4, VK_FORMAT_R32G32B32A32_SFLOAT} };

            format = mapped_format.at(accessor.type);

            break;
        }
        default: {
            format = VK_FORMAT_UNDEFINED;
            break;
        }
        }

        return format;
    }
    inline std::vector<uint8_t> getAttributeData(const tinygltf::Model* model, uint32_t accessorId)
    {
        auto& accessor = model->accessors.at(accessorId);
        auto& bufferView = model->bufferViews.at(accessor.bufferView);
        auto& buffer = model->buffers.at(bufferView.buffer);

        size_t stride = accessor.ByteStride(bufferView);
        size_t startByte = accessor.byteOffset + bufferView.byteOffset;
        size_t endByte = startByte + accessor.count * stride;

        return { buffer.data.begin() + startByte, buffer.data.begin() + endByte };
    };
    
    inline std::vector<uint8_t> convertUnderlyingDataStride(const std::vector<uint8_t>& src_data, uint32_t src_stride, uint32_t dst_stride)
    {
        auto elem_count = to_u32(src_data.size()) / src_stride;

        std::vector<uint8_t> result(elem_count * dst_stride);

        for (uint32_t idxSrc = 0, idxDst = 0;
            idxSrc < src_data.size() && idxDst < result.size();
            idxSrc += src_stride, idxDst += dst_stride)
        {
            std::copy(src_data.begin() + idxSrc, src_data.begin() + idxSrc + src_stride, result.begin() + idxDst);
        }

        return result;
    }
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

        auto pair = std::make_pair("vertexBuffer", std::move(buffer));
        auto pSubmesh = std::make_unique<SubMesh>();
        pSubmesh->m_vertexBuffers.insert(std::move(pair));

        auto& commandBuffer = m_deviceManager->requestCommandBuffer();
        commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        if (gltfPrimitive.indices >= 0)
        {
            pSubmesh->m_vertexIndices = to_u32(getAttributeSize(&m_model, gltfPrimitive.indices));
            auto format = getAttributeFormat(&m_model, gltfPrimitive.indices);
            auto indexData = getAttributeData(&m_model, gltfPrimitive.indices);

            switch (format)
            {
            case VK_FORMAT_R32_UINT: {
                // Correct format
                break;
            }
            case VK_FORMAT_R16_UINT: {
                indexData = convertUnderlyingDataStride(indexData, 2, 4);
                break;
            }
            case VK_FORMAT_R8_UINT: {
                indexData = convertUnderlyingDataStride(indexData, 1, 4);
                break;
            }
            default: {
                break;
            }
            }
            pSubmesh->m_indexType = VK_INDEX_TYPE_UINT32;
            RHI::Buffer indexStageBuffer{ m_deviceManager,
                indexData.size() * sizeof(uint8_t),
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VMA_MEMORY_USAGE_CPU_ONLY };
            indexStageBuffer.update(indexData.data(), indexData.size() * sizeof(uint8_t));
            pSubmesh->m_indexBuffer = std::make_unique<RHI::Buffer>(m_deviceManager,
                indexData.size(),
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VMA_MEMORY_USAGE_GPU_ONLY);
            commandBuffer.copyBuffer(indexStageBuffer, *pSubmesh->m_indexBuffer, indexData.size());
        }

        // todo : commandpool && commandbuffer, copy buffer

        // todo : store vkbuffer to submesh, to draw
        commandBuffer.copyBuffer(stageBuffer, buffer, vertexData.size() * sizeof(vertexData));
        commandBuffer.end();
        VkFence fence = m_deviceManager->requestFence();
        APP::VkDeviceManager::submit(m_deviceManager->getGraphicsQueue(), commandBuffer, fence);
        auto res = m_deviceManager->waitForFences({ fence });
        return std::move(pSubmesh);
    }
}
