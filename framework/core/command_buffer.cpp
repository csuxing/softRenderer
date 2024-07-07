#include "command_buffer.h"

#include "app/vk_device_manager.h"

#include "command_pool.h"
#include "core/image.h"
#include "buffer.h"
#include "error.h"

namespace RHI
{
    CommandBuffer::CommandBuffer(CommandPool* commandPool, VkCommandBufferLevel level) :
        m_commandPool(commandPool),
        m_level(level)
    {
        VkCommandBufferAllocateInfo allocationInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        allocationInfo.commandPool = m_commandPool->getHandle();
        allocationInfo.commandBufferCount = 1;
        allocationInfo.level = level;

        VkResult result = vkAllocateCommandBuffers(m_commandPool->getDeviceManager()->getDevice(), &allocationInfo, &m_handle);

        if (result != VK_SUCCESS)
        {
            throw VulkanException{ result, "Failed to allocate command buffer" };
        }
    }

    VkResult CommandBuffer::begin(VkCommandBufferUsageFlags flags, CommandBuffer* primaryCmdBuf)
    {
        if (m_level == VK_COMMAND_BUFFER_LEVEL_SECONDARY)
        {
            ASSERT(primaryCmdBuf && "A primary command buffer pointer must be provided when calling begin from a secondary one");
            // todo(xingyuhang) : secondary command in the feature
        }
        return begin(flags, nullptr, nullptr, 0);
    }
    VkResult CommandBuffer::begin(VkCommandBufferUsageFlags flags, const RenderPass* render_pass, const Framebuffer* framebuffer, uint32_t subpass_index)
    {
        ASSERT(!isRecording() && "Command buffer is already recording");
        if (isRecording())
        {
            return VK_NOT_READY;
        }
        m_state = State::Recording;
        // todo : reset state
        
        VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        VkCommandBufferInheritanceInfo inheritanceInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO };
        beginInfo.flags = flags;
        if (m_level == VK_COMMAND_BUFFER_LEVEL_SECONDARY)
        {
            // todo : secondary command buffer
        }
        return vkBeginCommandBuffer(m_handle, &beginInfo);
    }

    VkResult CommandBuffer::end()
    {
        assert(isRecording() && "Command buffer is not recording, please call begin before end");

        if (!isRecording())
        {
            return VK_NOT_READY;
        }

        vkEndCommandBuffer(m_handle);

        m_state = State::Executable;

        return VK_SUCCESS;
    }

    void CommandBuffer::copyBuffer(const Buffer& srcBuffer, const Buffer& dstBuffer, VkDeviceSize size)
    {
        VkBufferCopy copyRegion = {};
        copyRegion.size = size;
        vkCmdCopyBuffer(m_handle, srcBuffer.getHandle(), dstBuffer.getHandle(), 1, &copyRegion);
    }

    bool CommandBuffer::isRecording() const
    {
        return m_state == State::Recording;
    }

    void CommandBuffer::image_memory_barrier(const RHI::ImageView& image_view,
        const RHI::ImageMemoryBarrier& memory_barrier) const
    {
        // Adjust barrier's subresource range for depth images
        auto subresource_range = image_view.get_subresource_range();
        auto format = image_view.get_format();
        if (is_depth_only_format(format))
        {
            subresource_range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        else if (is_depth_stencil_format(format))
        {
            subresource_range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        }

        VkImageMemoryBarrier image_memory_barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        image_memory_barrier.oldLayout = memory_barrier.old_layout;
        image_memory_barrier.newLayout = memory_barrier.new_layout;
        image_memory_barrier.image = image_view.get_image().getHandle();
        image_memory_barrier.subresourceRange = subresource_range;
        image_memory_barrier.srcAccessMask = memory_barrier.src_access_mask;
        image_memory_barrier.dstAccessMask = memory_barrier.dst_access_mask;
        image_memory_barrier.srcQueueFamilyIndex = memory_barrier.old_queue_family;
        image_memory_barrier.dstQueueFamilyIndex = memory_barrier.new_queue_family;

        VkPipelineStageFlags src_stage_mask = memory_barrier.src_stage_mask;
        VkPipelineStageFlags dst_stage_mask = memory_barrier.dst_stage_mask;

        vkCmdPipelineBarrier(
            m_handle,
            src_stage_mask,
            dst_stage_mask,
            0,
            0, nullptr,
            0, nullptr,
            1,
            &image_memory_barrier);
    }
}
