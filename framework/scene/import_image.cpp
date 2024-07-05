#include "import_image.h"
#include "ktx.h"
#include "macro.h"
#include "fileSystem.h"

namespace SG
{
    static VkFormat maybe_coerce_to_srgb(VkFormat fmt)
    {
        switch (fmt)
        {
        case VK_FORMAT_R8_UNORM:
            return VK_FORMAT_R8_SRGB;
        case VK_FORMAT_R8G8_UNORM:
            return VK_FORMAT_R8G8_SRGB;
        case VK_FORMAT_R8G8B8_UNORM:
            return VK_FORMAT_R8G8B8_SRGB;
        case VK_FORMAT_B8G8R8_UNORM:
            return VK_FORMAT_B8G8R8_SRGB;
        case VK_FORMAT_R8G8B8A8_UNORM:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case VK_FORMAT_B8G8R8A8_UNORM:
            return VK_FORMAT_B8G8R8A8_SRGB;
        case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
            return VK_FORMAT_A8B8G8R8_SRGB_PACK32;
        case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
            return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
        case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
            return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
        case VK_FORMAT_BC2_UNORM_BLOCK:
            return VK_FORMAT_BC2_SRGB_BLOCK;
        case VK_FORMAT_BC3_UNORM_BLOCK:
            return VK_FORMAT_BC3_SRGB_BLOCK;
        case VK_FORMAT_BC7_UNORM_BLOCK:
            return VK_FORMAT_BC7_SRGB_BLOCK;
        case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
            return VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;
        case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
            return VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;
        case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
            return VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
        case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
            return VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
        case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
            return VK_FORMAT_ASTC_5x4_SRGB_BLOCK;
        case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
            return VK_FORMAT_ASTC_5x5_SRGB_BLOCK;
        case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
            return VK_FORMAT_ASTC_6x5_SRGB_BLOCK;
        case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
            return VK_FORMAT_ASTC_6x6_SRGB_BLOCK;
        case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
            return VK_FORMAT_ASTC_8x5_SRGB_BLOCK;
        case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
            return VK_FORMAT_ASTC_8x6_SRGB_BLOCK;
        case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
            return VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
        case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
            return VK_FORMAT_ASTC_10x5_SRGB_BLOCK;
        case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
            return VK_FORMAT_ASTC_10x6_SRGB_BLOCK;
        case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
            return VK_FORMAT_ASTC_10x8_SRGB_BLOCK;
        case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
            return VK_FORMAT_ASTC_10x10_SRGB_BLOCK;
        case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
            return VK_FORMAT_ASTC_12x10_SRGB_BLOCK;
        case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
            return VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
        case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:
            return VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG;
        case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:
            return VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG;
        case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:
            return VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG;
        case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:
            return VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG;
        default:
            return fmt;
        }
    }
    std::unique_ptr<ImportImage> ImportImage::load(const std::string& name, const std::string& url, ContentType content_type)
    {
        std::unique_ptr<ImportImage> image{ nullptr };

        auto data = Jerry::read_binary_file(url, 0);
        auto extension = Jerry::get_extension(name);
        if ((extension == "png") || (extension == "jpg"))
        {
            // image =  
        }
        else if (extension == "astc")
        {

        }
        else if (extension == "ktx")
        {
            image = std::make_unique<Ktx>(name, data, content_type);
        }
        else if (extension == "ktx2")
        {
            image = std::make_unique<Ktx>(name, data, content_type);
        }
        return image;
    }
    ImportImage::ImportImage(const std::string& name, std::vector<uint8_t>&& data, std::vector<Mipmap>&& mipmaps):
        m_name(name)
    {

    }
    std::type_index ImportImage::getType()
    {
        return typeid(ImportImage);
    }
    void ImportImage::coerce_format_to_srgb()
    {
        m_format = maybe_coerce_to_srgb(m_format);
    }
    void ImportImage::setData(const uint8_t* rawData, size_t size)
    {
        ASSERT(m_data.empty() && "image data already set!");
        m_data = { rawData, rawData + size };
    }
    std::vector<uint8_t>& ImportImage::getMutData()
    {
        return m_data;
    }
    void ImportImage::setWidth(uint32_t width)
    {
        m_mipMaps.at(0).extent.width = width;
    }
    void ImportImage::setHeight(uint32_t height)
    {
        m_mipMaps.at(0).extent.height = height;
    }
    void ImportImage::setDepth(uint32_t depth)
    {
        m_mipMaps.at(0).extent.depth = depth;
    }
    void ImportImage::setLayers(uint32_t layers)
    {
        m_layers = layers;
    }
    void ImportImage::setFormat(VkFormat format)
    {
        m_format = format;
    }
    void ImportImage::set_offsets(const std::vector<std::vector<VkDeviceSize>>& offsets)
    {
        m_offsets = offsets;
    }
    std::vector<Mipmap>& ImportImage::get_mut_mipmaps()
    {
        return m_mipMaps;
    }
}