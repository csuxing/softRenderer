#ifndef _COMMAND_BUFFER_H__
#define _COMMAND_BUFFER_H__
#include <volk.h>
namespace RHI
{
    class CommandPool;
    class RenderPass;
    class Framebuffer;
    class Buffer;
    class CommandBuffer
    {
    public:
        enum class ResetMode
        {
            ResetPool,
            ResetIndividually,
            AlwaysAllocate,
        };

        enum class State
        {
            Invalid,
            Initial,
            Recording,
            Executable,
        };
        CommandBuffer(CommandPool* commandPool, VkCommandBufferLevel level);
        VkResult begin(VkCommandBufferUsageFlags flags, CommandBuffer* primaryCmdBuf = nullptr);
        VkResult begin(VkCommandBufferUsageFlags flags, const RenderPass* render_pass, const Framebuffer* framebuffer, uint32_t subpass_index);
        VkResult end();
        void copyBuffer(const Buffer& srcBuffer, const Buffer& dstBuffer, VkDeviceSize size);
        bool isRecording() const;
        VkCommandBuffer getHandle() const noexcept { return m_handle; }
    protected:
    private:
        CommandPool*        m_commandPool{};
        VkCommandBufferLevel m_level;
        VkCommandBuffer     m_handle{};
        State               m_state{};

    };
}
#endif // _COMMAND_BUFFER_H__
