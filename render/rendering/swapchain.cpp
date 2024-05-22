#include "swapchain.h"
#include "core/device.h"
#include "core/error.h"
namespace RHI
{
    Swapchain::Swapchain(Device& device, VkSurfaceKHR surface, const VkExtent2D& extent
        , const uint32_t image_count, const VkSurfaceTransformFlagBitsKHR transform
        , const VkPresentModeKHR present_mode, const std::set<VkImageUsageFlagBits>& image_usage_flags) : 
        Swapchain(*this, device, surface, extent, image_count, transform, present_mode, image_usage_flags)
    {

    }
    Swapchain::Swapchain(Swapchain& oldSwapchain, 
        Device& device, VkSurfaceKHR surface, const VkExtent2D& extent, const uint32_t image_count, 
        const VkSurfaceTransformFlagBitsKHR transform, const VkPresentModeKHR present_mode, 
        const std::set<VkImageUsageFlagBits>& image_usage_flags) : 
        m_device(device),
        m_surface(surface),
        m_extent(extent),
        m_imageCount(image_count),
        m_transform(transform),
        m_presentMode(present_mode),
        m_imageUsageFlags(image_usage_flags)
    {
        m_presentModePriorityList   = oldSwapchain.getPresentModeList();
        m_surfaceFormatPriorityList = oldSwapchain.getSurfaceFormatList();

        VkSurfaceCapabilitiesKHR surfaceCapblities{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device.getGpu().getHandle(), surface, &surfaceCapblities);

        uint32_t surface_format_count{ 0U };
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_device.getGpu().getHandle(), surface, &surface_format_count, nullptr));
        m_surfaceFormats.resize(surface_format_count);
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_device.getGpu().getHandle(), surface, &surface_format_count, m_surfaceFormats.data()));


    }
}