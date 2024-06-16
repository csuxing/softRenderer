#include "image_view.h"
#include "image.h"
#include "macro.h"
#include "app/vk_device_manager.h"

namespace RHI
{
    inline bool isDepthOnlyFormat(VkFormat format)
    {
        return format == VK_FORMAT_D16_UNORM ||
            format == VK_FORMAT_D32_SFLOAT;
    }

    inline bool isDepthStencilFormat(VkFormat format)
    {
        return format == VK_FORMAT_D16_UNORM_S8_UINT ||
            format == VK_FORMAT_D24_UNORM_S8_UINT ||
            format == VK_FORMAT_D32_SFLOAT_S8_UINT;
    }

    ImageView::ImageView(APP::VkDeviceManager* deviceManger, Image* image, VkImageViewType viewType, VkFormat format,
        uint32_t mipLevel, uint32_t arrayLayer, uint32_t nMipLevels, uint32_t nArrayLayers) :
    m_deviceManager(deviceManger),
    m_image(image),
    m_viewType(viewType),
    m_format(format)
    {
        if (format == VK_FORMAT_UNDEFINED)
        {
            m_format = image->getFormat();
        }

        m_subresourceRange.baseMipLevel = mipLevel;
        m_subresourceRange.baseArrayLayer = arrayLayer;
        m_subresourceRange.levelCount = nMipLevels == 0 ? m_image->getImageSubresource().mipLevel : nMipLevels;
        m_subresourceRange.layerCount = nArrayLayers == 0 ? m_image->getImageSubresource().arrayLayer : nArrayLayers;

        if (isDepthStencilFormat(m_format))
        {
            m_subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        }
        else if (isDepthOnlyFormat(m_format))
        {
            m_subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        else
        {
            m_subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        VkImageViewCreateInfo viewInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        viewInfo.image = m_image->getHandle();
        viewInfo.viewType = m_viewType;
        viewInfo.format = m_format;
        viewInfo.subresourceRange = m_subresourceRange;

        auto res = vkCreateImageView(m_deviceManager->getDevice(), &viewInfo, nullptr, &m_handle);
        if (res != VK_SUCCESS)
        {
            LOG_ERROR("create image view faied!");
        }
    }
}
