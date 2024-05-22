#ifndef _RENDER_CONTEXT_H__
#define _RENDER_CONTEXT_H__
#include <volk.h>
#include <GLFW/glfw3.h>
namespace RHI
{
    class CommandBuffer;
    class Device;
    class RenderContext
    {
    public:
        RenderContext(Device& device, VkSurfaceKHR surface, const GLFWwindow* window);
        void submit(CommandBuffer& commandBuffer);
        void beginFrame();
        void endFrame();
    private:
        Device&             m_device;
        VkSurfaceKHR        m_surface{ VK_NULL_HANDLE };
        const GLFWwindow*   m_window{ nullptr };
    };
}

#endif // _RENDER_CONTEXT_H__
