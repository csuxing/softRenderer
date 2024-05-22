#ifndef _SWAPCHAIN_H_H_
#define _SWAPCHAIN_H_H_

#include <volk.h>
#include <set>
#include <vector>
namespace RHI
{
    class Device;
    class Swapchain
    {
    public:
        Swapchain(Device&                         device,
            VkSurfaceKHR                          surface,
            const VkExtent2D&                     extent = {},
            const uint32_t                        image_count = 3,
            const VkSurfaceTransformFlagBitsKHR   transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            const VkPresentModeKHR                present_mode = VK_PRESENT_MODE_FIFO_KHR,
            const std::set<VkImageUsageFlagBits>& image_usage_flags = { VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_USAGE_TRANSFER_SRC_BIT });
    
        Swapchain(Swapchain&                      oldSwapchain,
            Device&                               device,
            VkSurfaceKHR                          surface,
            const VkExtent2D&                     extent = {},
            const uint32_t                        image_count = 3,
            const VkSurfaceTransformFlagBitsKHR   transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            const VkPresentModeKHR                present_mode = VK_PRESENT_MODE_FIFO_KHR,
            const std::set<VkImageUsageFlagBits>& image_usage_flags = { VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_USAGE_TRANSFER_SRC_BIT });
    
        // get && set
        const std::vector<VkPresentModeKHR>&    getPresentModeList() const { return m_presentModePriorityList; }
        const std::vector<VkSurfaceFormatKHR>&  getSurfaceFormatList() const { return m_surfaceFormatPriorityList; }
    private:
        VkSwapchainKHR                      m_swapchain{ VK_NULL_HANDLE };
        Device&                             m_device;
        VkSurfaceKHR                        m_surface{};
        VkExtent2D                          m_extent{};
        uint32_t                            m_imageCount{};
        VkSurfaceTransformFlagBitsKHR       m_transform{};
        VkPresentModeKHR                    m_presentMode{};
        std::set<VkImageUsageFlagBits>      m_imageUsageFlags{};
        
        std::vector<VkSurfaceFormatKHR>     m_surfaceFormats{};
        std::vector<VkPresentModeKHR>       m_presentModes{};
        VkSwapchainKHR                      m_handle{};

        // preDefine
        std::vector<VkPresentModeKHR> m_presentModePriorityList = {
            VK_PRESENT_MODE_FIFO_KHR,
            VK_PRESENT_MODE_MAILBOX_KHR 
        };
        std::vector<VkSurfaceFormatKHR> m_surfaceFormatPriorityList = {
            {VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
            {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}
        };
    };
    
}

#endif
