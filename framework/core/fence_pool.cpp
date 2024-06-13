#include "fence_pool.h"
#include "error.h"
#include "app/vk_device_manager.h"

namespace RHI
{
    FencePool::FencePool(APP::VkDeviceManager* deviceManager):
        m_deviceManager(deviceManager)
    {

    }
    VkFence FencePool::requestFence()
    {
        if (m_activeFenceCount < m_fences.size())
        {
            return m_fences.at(m_activeFenceCount++);
        }
        VkFence fence{ VK_NULL_HANDLE };
        VkFenceCreateInfo createInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        VkResult result = vkCreateFence(m_deviceManager->getDevice(), &createInfo, nullptr, &fence);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create fence");
        }
        m_fences.push_back(fence);
        m_activeFenceCount++;

        return m_fences.back();
    }
}