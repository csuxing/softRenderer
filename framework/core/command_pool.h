#ifndef _COMMAND_POOL_H__
#define _COMMAND_POOL_H__
#include <cstdint>
#include <volk.h>

#include "command_buffer.h"
namespace RHI
{
    class RenderFrame;
    class Device;
    class CommandPool
    {
    public:
        CommandPool(Device& device, uint32_t queueFamilyIndex, RenderFrame* render_frame = nullptr,
            size_t                   thread_index = 0,
            CommandBuffer::ResetMode reset_mode = CommandBuffer::ResetMode::ResetPool);

        Device& getDevice()
        {
            return m_device;
        }

        uint32_t getQueueFamilyIndex() const noexcept
        {
            return m_queueFamilyIndex;
        }

        RenderFrame* getRenderFrame()
        {
            return m_renderFrame;
        }

        size_t getThreadIndex() const noexcept
        {
            return m_threadIndex;
        }

        VkCommandPool getHandle() const noexcept
        {
            return m_handle;
        }

        // todo : requst command buffer form command pool
    private:
        Device&                          m_device;
        uint32_t                         m_queueFamilyIndex{ 0 };
        RenderFrame*                     m_renderFrame{ nullptr };
        size_t                           m_threadIndex{ 0 };
        CommandBuffer::ResetMode         m_resetMode{};
        VkCommandPool                    m_handle{ VK_NULL_HANDLE };
    };
}

#endif // _COMMAND_POOL_H__
