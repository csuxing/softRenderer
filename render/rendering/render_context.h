#ifndef _RENDER_CONTEXT_H__
#define _RENDER_CONTEXT_H__
#include <volk.h>
#include <GLFW/glfw3.h>
#include <vector>

namespace RHI
{
    class CommandBuffer;
    class Device;
    class Swapchain;
    class RenderContext
    {
    public:
        struct PerFrame
        {
            VkDevice device = VK_NULL_HANDLE;

            VkFence queue_submit_fence = VK_NULL_HANDLE;

            VkCommandPool primary_command_pool = VK_NULL_HANDLE;

            VkCommandBuffer primary_command_buffer = VK_NULL_HANDLE;

            VkSemaphore swapchain_acquire_semaphore = VK_NULL_HANDLE;

            VkSemaphore swapchain_release_semaphore = VK_NULL_HANDLE;

            int32_t queue_index;
        };

        RenderContext(Device& device, VkSurfaceKHR surface, const GLFWwindow* window);
        void init();
        void submit(CommandBuffer& commandBuffer);
        void beginFrame();
        void endFrame();

    protected:
        void initRenderpass();
        void initPipeline();
        void initFrameBuffer();
    private:
        Device&             m_device;
        VkSurfaceKHR        m_surface{ VK_NULL_HANDLE };
        const GLFWwindow*   m_window{ nullptr };
        Swapchain*          m_swapChain{ nullptr };

        // temp code for primary triangle test
        VkRenderPass                m_renderpass{ VK_NULL_HANDLE };
        std::vector<VkFramebuffer>  m_frameBuffers{};
        VkPipelineLayout            m_pipelineLayout{};
        VkPipeline                  m_pipeline{};
        std::vector<PerFrame> m_perframes;

    };
}

#endif // _RENDER_CONTEXT_H__
