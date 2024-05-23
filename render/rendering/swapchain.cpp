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
        m_presentModePriorityList = oldSwapchain.getPresentModeList();
        m_surfaceFormatPriorityList = oldSwapchain.getSurfaceFormatList();

        VkSurfaceCapabilitiesKHR surfaceCapblities{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device.getGpu().getHandle(), surface, &surfaceCapblities);

        uint32_t surface_format_count{ 0U };
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_device.getGpu().getHandle(), surface, &surface_format_count, nullptr));
        m_surfaceFormats.resize(surface_format_count);
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_device.getGpu().getHandle(), surface, &surface_format_count, m_surfaceFormats.data()));

        VkSurfaceFormatKHR format;

        format = m_surfaceFormats[0];
        format.format = VK_FORMAT_B8G8R8A8_SRGB;

        m_currentUse = format;

        VkExtent2D swapchain_size;
        if (surfaceCapblities.currentExtent.width == 0xFFFFFFFF)
        {
            swapchain_size.width = extent.width;
            swapchain_size.height = extent.height;
        }
        else
        {
            swapchain_size = surfaceCapblities.currentExtent;
        }
        m_extent = swapchain_size;
        VkPresentModeKHR swapchain_present_mode = VK_PRESENT_MODE_FIFO_KHR;
        uint32_t desired_swapchain_images = surfaceCapblities.minImageCount + 1;
        if ((surfaceCapblities.maxImageCount > 0) && (desired_swapchain_images > surfaceCapblities.maxImageCount))
        {
            // Application must settle for fewer images than desired.
            desired_swapchain_images = surfaceCapblities.maxImageCount;
        }

        VkSurfaceTransformFlagBitsKHR pre_transform;
        if (surfaceCapblities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
        {
            pre_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        }
        else
        {
            pre_transform = surfaceCapblities.currentTransform;
        }

        VkCompositeAlphaFlagBitsKHR composite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        if (surfaceCapblities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
        {
            composite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        }
        else if (surfaceCapblities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
        {
            composite = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
        }
        else if (surfaceCapblities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
        {
            composite = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
        }
        else if (surfaceCapblities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
        {
            composite = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
        }

        VkSwapchainCreateInfoKHR info{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        info.surface = m_surface;
        info.minImageCount = desired_swapchain_images;
        info.imageFormat = format.format;
        info.imageColorSpace = format.colorSpace;
        info.imageExtent.width = swapchain_size.width;
        info.imageExtent.height = swapchain_size.height;
        info.imageArrayLayers = 1;
        info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.preTransform = pre_transform;
        info.compositeAlpha = composite;
        info.presentMode = swapchain_present_mode;
        info.clipped = true;
        info.oldSwapchain = oldSwapchain.m_handle;

        VK_CHECK(vkCreateSwapchainKHR(m_device.get_handle(), &info, nullptr, &m_handle));

        uint32_t imageCount;
        VK_CHECK(vkGetSwapchainImagesKHR(m_device.get_handle(), m_handle, &imageCount, nullptr));
        m_swapChainImages.resize(imageCount);
        VK_CHECK(vkGetSwapchainImagesKHR(m_device.get_handle(), m_handle, &imageCount, m_swapChainImages.data()));
        for (uint32_t i = 0; i < imageCount; ++i)
        {
            VkImageViewCreateInfo view_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
            view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            view_info.format = format.format;
            view_info.image = m_swapChainImages[i];
            view_info.subresourceRange.levelCount = 1;
            view_info.subresourceRange.layerCount = 1;
            view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            view_info.components.r = VK_COMPONENT_SWIZZLE_R;
            view_info.components.g = VK_COMPONENT_SWIZZLE_G;
            view_info.components.b = VK_COMPONENT_SWIZZLE_B;
            view_info.components.a = VK_COMPONENT_SWIZZLE_A;

            VkImageView image_view;
            VK_CHECK(vkCreateImageView(m_device.get_handle(), &view_info, nullptr, &image_view));
            m_swapChainImageViews.push_back(image_view);
        }
    }
}