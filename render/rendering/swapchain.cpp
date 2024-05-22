#include "swapchain.h"
namespace RHI
{
    Swapchain::Swapchain(Device& device, VkSurfaceKHR surface, const VkExtent2D& extent
        , const uint32_t image_count, const VkSurfaceTransformFlagBitsKHR transform
        , const VkPresentModeKHR present_mode, const std::set<VkImageUsageFlagBits>& image_usage_flags) : 
        m_device(device),
        m_surface(surface),
        m_extent(extent),
        m_imageCount(image_count),
        m_transform(transform),
        m_presentMode(present_mode),
        m_imageUsageFlags(image_usage_flags)
    {

    }
}