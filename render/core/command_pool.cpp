#include "command_pool.h"
#include "device.h"
#include "error.h"
namespace RHI
{
    CommandPool::CommandPool(Device& device, uint32_t queueFamilyIndex, RenderFrame* render_frame, size_t thread_index, CommandBuffer::ResetMode reset_mode) : 
        m_device(device),
        m_queueFamilyIndex(queueFamilyIndex),
        m_renderFrame(render_frame),
        m_threadIndex(thread_index),
        m_resetMode(reset_mode)
    {
        VkCommandPoolCreateFlags flags;
        switch (reset_mode)
        {
        case CommandBuffer::ResetMode::ResetIndividually:
        case CommandBuffer::ResetMode::AlwaysAllocate:
            flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            break;
        case CommandBuffer::ResetMode::ResetPool:
        default:
            flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
            break;
        }

        VkCommandPoolCreateInfo create_info{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };

        create_info.queueFamilyIndex = queueFamilyIndex;
        create_info.flags = flags;

        auto result = vkCreateCommandPool(device.get_handle(), &create_info, nullptr, &m_handle);

        if (result != VK_SUCCESS)
        {
            throw VulkanException{ result, "Failed to create command pool" };
        }
    }
}