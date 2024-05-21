#ifndef _DEVICE_H__
#define _DEVICE_H__
#include <cstdint>
#include <unordered_map>
#include <volk.h>
#include "vk_mem_alloc.h"

#include "physical_device.h"
#include "vulkan_resource.h"
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
    protected:

    private:
        const PhysicalDevice& m_gpu;
        VmaAllocator m_memoryAllocator{ VK_NULL_HANDLE };
    };
}

#endif // _DEVICE_H__
