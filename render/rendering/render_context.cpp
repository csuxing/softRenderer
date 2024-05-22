#include "render_context.h"
#include "core/device.h"
#include "core/error.h"
#include "swapchain.h"
namespace RHI
{
    RenderContext::RenderContext(Device& device, VkSurfaceKHR surface, const GLFWwindow* window) : 
        m_device(device),
        m_surface(surface),
        m_window(window)
    {
        if (surface != VK_NULL_HANDLE)
        {
            VkSurfaceCapabilitiesKHR surfaceProperties;
            VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.getGpu().getHandle(), 
                surface,
                &surfaceProperties));
            if (surfaceProperties.currentExtent.width == 0xFFFFFFFF)
            {

            }
        }
    }
    void RenderContext::submit(CommandBuffer& commandBuffer)
    {

    }
}