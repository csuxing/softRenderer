#ifndef _COMMAND_POOL_H__
#define _COMMAND_POOL_H__
#include <cstdint>

#include <vector>
#include <memory>
#include <volk.h>

#include "command_buffer.h"
namespace APP
{
    class VkDeviceManager;
}
namespace RHI
{
    class RenderFrame;

    class CommandPool
    {
    public:
        CommandPool(APP::VkDeviceManager* deviceManager, uint32_t queueFamilyIndex, RenderFrame* renderFrame = nullptr,
            size_t threadIndex = 0, CommandBuffer::ResetMode resetMode = CommandBuffer::ResetMode::ResetPool);

        CommandBuffer& requestCommandBuffer(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        VkCommandPool getHandle() const noexcept { return m_handle; }
        APP::VkDeviceManager* getDeviceManager() const noexcept { return m_deviceManager; }
    private:
        APP::VkDeviceManager*                           m_deviceManager{};
        VkCommandPool                                   m_handle{};

        std::vector<std::unique_ptr<CommandBuffer>>     m_primaryCommandBuffers;
        uint32_t                                        m_activePrimaryCommandBufferCount{ 0 };
        std::vector<std::unique_ptr<CommandBuffer>>     m_secondaryCommandBuffers;
        uint32_t                                        m_activeSecondaryCommandBufferCount{ 0 };
    };
}

#endif // _COMMAND_POOL_H__
