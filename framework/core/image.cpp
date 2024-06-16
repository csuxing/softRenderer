#include "image.h"

#include "macro.h"
#include "app/vk_device_manager.h"

namespace RHI
{
    Image::Image(APP::VkDeviceManager* device, const VkExtent3D extent, VkFormat format, VkImageUsageFlags usageFlags,
        VmaMemoryUsage memoryUsage, VkSampleCountFlagBits sampleCount, uint32_t mipLevels, uint32_t arrayLayers,
        VkImageTiling tiling, VkImageCreateFlags flags, uint32_t numQueueFamilies, const uint32_t* queueFamilies) :
        m_deviceManager(device),
        m_extent(extent),
        m_format(format),
        m_usage(usageFlags),
        m_sampleCount(sampleCount),
        m_tiling(tiling),
        m_arrayLayerCount(arrayLayers)
    {
        // todo : now only support image2d
        m_type = VK_IMAGE_TYPE_2D;
        ASSERT(mipLevels > 0 && "at least one level");
        ASSERT(arrayLayers > 0 && "at least one layer");

        m_subresource.mipLevel = mipLevels;
        m_subresource.arrayLayer = arrayLayers;

        VkImageCreateInfo imageCi{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        imageCi.flags = flags;
        imageCi.imageType = m_type;
        imageCi.format = m_format;
        imageCi.extent = m_extent;
        imageCi.mipLevels = mipLevels;
        imageCi.arrayLayers = m_arrayLayerCount;
        imageCi.samples = m_sampleCount;
        imageCi.tiling = m_tiling;
        imageCi.usage = m_usage;

        if (numQueueFamilies != 0)
        {
            imageCi.sharingMode = VK_SHARING_MODE_CONCURRENT;
            imageCi.queueFamilyIndexCount = numQueueFamilies;
            imageCi.pQueueFamilyIndices = queueFamilies;
        }

        VmaAllocationCreateInfo memoryInfo{};
        memoryInfo.usage = memoryUsage;

        if (usageFlags & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)
        {
            memoryInfo.preferredFlags = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
        }

        auto result = vmaCreateImage(m_deviceManager->getVmaAllocator(),
            &imageCi, &memoryInfo, &m_handle, &m_memory, nullptr);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("create image failed!");
        }
    }
}
