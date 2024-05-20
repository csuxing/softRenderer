#ifndef _DEVICE_H__
#define _DEVICE_H__
#include <cstdint>
#include <unordered_map>
#include <volk.h>

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
    protected:

    private:

    };
}

#endif // _DEVICE_H__
