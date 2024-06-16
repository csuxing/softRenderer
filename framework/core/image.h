#ifndef _IMAGE_H_H_
#define _IMAGE_H_H_
#include <vk_mem_alloc.h>
#include <volk.h>
namespace APP
{
    class VkDeviceManager;
}
namespace RHI
{
    class Image
    {
    public:
        Image(APP::VkDeviceManager* device,
            const VkExtent3D extent,
            VkFormat format,
            VkImageUsageFlags usageFlags,
            VmaMemoryUsage memoryUsage,
            VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT,
            uint32_t mipLevels = 1,
            uint32_t arrayLayers = 1,
            VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL,
            VkImageCreateFlags flags = 0,
            uint32_t numQueueFamilies = 0,
            const uint32_t* queueFamilies = nullptr);
        VkFormat getFormat() const { return m_format; }
        VkImageSubresource getImageSubresource() const { return m_subresource; }
        VkImage getHandle() const { return m_handle; }

    private:
        APP::VkDeviceManager* m_deviceManager{ nullptr };
        VkImage m_handle{};
        VmaAllocation m_memory{};
        VkImageType m_type{};
        VkExtent3D m_extent{};
        VkFormat m_format{};
        VkImageUsageFlags m_usage{};
        VkSampleCountFlagBits m_sampleCount{};
        VkImageTiling m_tiling{};
        VkImageSubresource m_subresource{};
        uint32_t m_arrayLayerCount{};

        uint8_t* m_mappedData{};
        bool m_mapped{ false };
    };
}

#endif
