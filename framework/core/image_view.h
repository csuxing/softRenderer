#ifndef _IMAGE_VIEW_H_H_
#define _IMAGE_VIEW_H_H_
#include <volk.h>

namespace APP
{
    class VkDeviceManager;
}
namespace RHI
{
    class Image;
    class ImageView
    {
    public:
        ImageView(APP::VkDeviceManager* deviceManger, Image* image, VkImageViewType viewType, VkFormat format,
            uint32_t mipLevel = 0, uint32_t arrayLayer = 0,
            uint32_t nMipLevels = 0, uint32_t nArrayLayers = 0);

        VkImageView getHandle() const { return m_handle; }
    private:
        APP::VkDeviceManager* m_deviceManager{};
        VkImageView             m_handle{};
        Image* m_image{ nullptr };
        VkImageViewType m_viewType{};
        VkFormat        m_format{};
        VkImageSubresourceRange m_subresourceRange{};
    };
}
#endif
