#ifndef _SWAPCHAIN_H_H_
#define _SWAPCHAIN_H_H_

#include <volk.h>
#include <set>
namespace RHI
{
    class Device;
    class Swapchain
    {
    public:
        Swapchain(Device& device,
            VkSurfaceKHR                          surface,
            const VkExtent2D& extent = {},
            const uint32_t                        image_count = 3,
            const VkSurfaceTransformFlagBitsKHR   transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            const VkPresentModeKHR                present_mode = VK_PRESENT_MODE_FIFO_KHR,
            const std::set<VkImageUsageFlagBits>& image_usage_flags = { VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_USAGE_TRANSFER_SRC_BIT });
    
    private:
        VkSwapchainKHR                      m_swapchain{ VK_NULL_HANDLE };
        Device&                             m_device;
        VkSurfaceKHR                        m_surface{};
        VkExtent2D                          m_extent{};
        uint32_t                            m_imageCount{};
        VkSurfaceTransformFlagBitsKHR       m_transform{};
        VkPresentModeKHR                    m_presentMode{};
        std::set<VkImageUsageFlagBits>      m_imageUsageFlags{};
    };
    
}

#endif
