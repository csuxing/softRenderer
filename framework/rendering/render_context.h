#ifndef _RENDER_CONTEXT_H__
#define _RENDER_CONTEXT_H__
#include <volk.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "app/vk_device_manager.h"

namespace APP
{
    class VkDeviceManager;
}
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

        RenderContext(APP::VkDeviceManager* deviceManager);

        void init();
        void submit(CommandBuffer& commandBuffer);
        void beginFrame();
        void frame();
        void endFrame();

    protected:
        void initRenderpass();
        void initPipeline();
        void initFrameBuffer();
    private:
        APP::VkDeviceManager*       m_deviceManager{nullptr};
        VkSurfaceKHR                m_surface{ VK_NULL_HANDLE };
        VkDevice                    m_device{ VK_NULL_HANDLE };
        VkQueue                     m_graphicsQueue{ VK_NULL_HANDLE };
        APP::SwapchainInfo          m_swapchainInfo;

        // temp code for primary triangle test
        VkRenderPass                m_renderpass{ VK_NULL_HANDLE };
        std::vector<VkFramebuffer>  m_frameBuffers{};
        VkPipelineLayout            m_pipelineLayout{};
        VkPipeline                  m_pipeline{};
        std::vector<PerFrame>       m_perframes;
        std::vector<VkSemaphore>    m_recycledSemaphores;
        uint32_t                    m_currentIndex{0};
    };
}

#endif // _RENDER_CONTEXT_H__
