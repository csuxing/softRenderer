#ifndef _DEVICE_H__
#define _DEVICE_H__
#include <cstdint>
#include <unordered_map>
#include <volk.h>
#include "vk_mem_alloc.h"

#include "physical_device.h"
#include "vulkan_resource.h"
#include "queue.h"
#include "command_pool.h"
#include "fence_pool.h"
namespace RHI
{
    struct DriverVersion
    {
        uint16_t major;
        uint16_t minor;
        uint16_t patch;
    };

    class Device : public VulkanResource<VkDevice, VkObjectType::VK_OBJECT_TYPE_DEVICE>
    {
    public:
        Device(PhysicalDevice& gpu,
            VkSurfaceKHR                           surface,
            std::unordered_map<const char*, bool> requested_extensions = {});

        uint32_t get_queue_family_index(VkQueueFlagBits queue_flag);
        bool isExtensionSupported(const std::string& extension) const;
        const Queue& getQueueByFlags(VkQueueFlags queue_flags, uint32_t queue_index) const;
        const PhysicalDevice& getGpu() const noexcept
        {
            return m_gpu;
        }
    protected:

    private:
        std::vector<VkExtensionProperties>  m_deviceExtensions;
        std::vector<const char*>            m_enabledExtensions;
        const PhysicalDevice&               m_gpu;
        VmaAllocator                        m_memoryAllocator{ VK_NULL_HANDLE };
        std::vector<std::vector<Queue>>     m_queues;
        std::unique_ptr<CommandPool>        m_commandPool{};
        std::unique_ptr<FencePool>          m_fencePool{};
    };
}

#endif // _DEVICE_H__
