#include "device.h"
namespace RHI
{
    Device::Device(PhysicalDevice& gpu, VkSurfaceKHR surface, std::unordered_map<const char*, bool> requested_extensions)
        : VulkanResource{VK_NULL_HANDLE, this}
    {

    }
}