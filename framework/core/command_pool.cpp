#include "command_pool.h"

#include <vulkan/vulkan_core.h>

#include "app/vk_device_manager.h"

#include "error.h"

namespace RHI
{
    CommandPool::CommandPool(APP::VkDeviceManager* deviceManager, uint32_t queueFamilyIndex, RenderFrame* renderFrame, size_t threadIndex, CommandBuffer::ResetMode resetMode):
        m_deviceManager(deviceManager)
    {
        VkCommandPoolCreateFlags flags;
        switch (resetMode)
        {
        case RHI::CommandBuffer::ResetMode::ResetIndividually:
        case RHI::CommandBuffer::ResetMode::AlwaysAllocate:
            flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            break;
        case RHI::CommandBuffer::ResetMode::ResetPool:
        default:
            flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
            break;
        }

        VkCommandPoolCreateInfo createInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        createInfo.queueFamilyIndex = queueFamilyIndex;
        createInfo.flags = flags;

        auto result = vkCreateCommandPool(m_deviceManager->getDevice(), &createInfo, nullptr, &m_handle);
        if (result != VK_SUCCESS)
        {
            throw VulkanException{ result, "Failed to create command pool" };
        }
    }
    CommandBuffer& CommandPool::requestCommandBuffer(VkCommandBufferLevel level)
    {
        if (level == VK_COMMAND_BUFFER_LEVEL_PRIMARY)
        {
            if (m_activePrimaryCommandBufferCount < m_primaryCommandBuffers.size())
            {
                return *m_primaryCommandBuffers.at(m_activePrimaryCommandBufferCount++);
            }
            m_primaryCommandBuffers.emplace_back(std::make_unique<CommandBuffer>(this, level));
            m_activePrimaryCommandBufferCount++;
            return *m_primaryCommandBuffers.back();
        }
        else
        {
            if (m_activeSecondaryCommandBufferCount < m_secondaryCommandBuffers.size())
            {
                return *m_secondaryCommandBuffers.at(m_activeSecondaryCommandBufferCount++);
            }
            m_secondaryCommandBuffers.emplace_back(std::make_unique<CommandBuffer>(this, level));
            m_activeSecondaryCommandBufferCount++;
            return *m_secondaryCommandBuffers.back();
        }
    }
}