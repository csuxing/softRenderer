#include "ktx.h"

#include <stdexcept>

#include <ktx.h>
#include <ktxvulkan.h>

#include "macro.h"

namespace SG
{
    struct CallbackData final
    {
        ktxTexture* texture;
        std::vector<Mipmap>* mipmaps;
    };
    static ktx_error_code_e KTX_APIENTRY optimal_tiling_callback(int          mip_level,
        int          face,
        int          width,
        int          height,
        int          depth,
        ktx_uint64_t face_lod_size,
        void* pixels,
        void* user_data)
    {
        auto* callback_data = reinterpret_cast<CallbackData*>(user_data);
        ASSERT(static_cast<size_t>(mip_level) < callback_data->mipmaps->size() && "Not enough space in the mipmap vector");

        ktx_size_t mipmap_offset = 0;
        auto       result = ktxTexture_GetImageOffset(callback_data->texture, mip_level, 0, face, &mipmap_offset);
        if (result != KTX_SUCCESS)
        {
            return result;
        }

        auto& mipmap = callback_data->mipmaps->at(mip_level);
        mipmap.level = mip_level;
        mipmap.offset = static_cast<uint32_t>(mipmap_offset);
        mipmap.extent.width = width;
        mipmap.extent.height = height;
        mipmap.extent.depth = depth;

        return KTX_SUCCESS;
    }
    SG::Ktx::Ktx(const std::string& name, const std::vector<uint8_t>& data, ContentType content_type):
        ImportImage{ name, {}, {} }
    {
        auto dataBuffer = reinterpret_cast<const ktx_uint8_t*>(data.data());
        auto dataSize = static_cast<ktx_size_t>(data.size());

        ktxTexture* texture;
        auto loadKtxResult = ktxTexture_CreateFromMemory(dataBuffer,
            dataSize,
            KTX_TEXTURE_CREATE_NO_FLAGS,
            &texture);

        if (loadKtxResult != KTX_SUCCESS)
        {
            throw std::runtime_error{ "Error loading KTX texture: " + name };
        }

        if (texture->pData)
        {
            setData(texture->pData, texture->dataSize);
        }
        else
        {
            auto& mutData = getMutData();
            auto size = texture->dataSize;
            mutData.resize(size);
            auto loadDataResult = ktxTexture_LoadImageData(texture, mutData.data(), size);
            if (loadDataResult != KTX_SUCCESS)
            {
                throw std::runtime_error{ "Error loading KTX image data: " + name };
            }
        }
        setWidth(texture->baseWidth);
        setHeight(texture->baseHeight);
        setDepth(texture->baseDepth);
        setLayers(texture->numLayers);

        bool cubeMap = false;
        if (texture->numLayers == 1 && texture->numFaces == 6)
        {
            cubeMap = true;
            setLayers(texture->numFaces);
        }

        auto updateFormat = ktxTexture_GetVkFormat(texture);
        setFormat(updateFormat);

        if (texture->classId == ktxTexture1_c && content_type == Color)
        {
            coerce_format_to_srgb();
        }

        auto& mipmap_levels = get_mut_mipmaps();
        mipmap_levels.resize(texture->numLevels);

        CallbackData callback_data{};
        callback_data.texture = texture;
        callback_data.mipmaps = &mipmap_levels;

        auto result = ktxTexture_IterateLevels(texture, optimal_tiling_callback, &callback_data);
        if (result != KTX_SUCCESS)
        {
            throw std::runtime_error("Error loading KTX texture");
        }

        if (texture->numLayers > 1 || cubeMap)
        {
            uint32_t layerCount = cubeMap ? texture->numFaces : texture->numLayers;
            std::vector<std::vector<VkDeviceSize>> offsets;
            for (uint32_t layer = 0; layer < layerCount; layer++)
            {
                std::vector<VkDeviceSize> layer_offsets{};
                for (uint32_t level = 0; level < texture->numLevels; level++)
                {
                    ktx_size_t     offset;
                    KTX_error_code result;
                    if (cubeMap)
                    {
                        result = ktxTexture_GetImageOffset(texture, level, 0, layer, &offset);
                    }
                    else
                    {
                        result = ktxTexture_GetImageOffset(texture, level, layer, 0, &offset);
                    }
                    layer_offsets.push_back(static_cast<VkDeviceSize>(offset));
                }
                offsets.push_back(layer_offsets);
            }
            set_offsets(offsets);
        }
        else
        {
            std::vector<std::vector<VkDeviceSize>> offsets{};
            offsets.resize(1);
            for (size_t level = 0; level < mipmap_levels.size(); level++)
            {
                offsets[0].push_back(static_cast<VkDeviceSize>(mipmap_levels[level].offset));
            }
            set_offsets(offsets);
        }

        ktxTexture_Destroy(texture);
    }
}
