#include "physical_device.h"
#include "../macro.h"
#include "error.h"
namespace RHI
{
    PhysicalDevice::PhysicalDevice(std::shared_ptr<Instance> instance, VkPhysicalDevice physical_device)
        : m_instance(instance),
          m_handle(physical_device)
    {
        vkGetPhysicalDeviceFeatures(physical_device, &features);
        vkGetPhysicalDeviceProperties(physical_device, &properties);
        vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);

        LOGI("Found GPU : {}", properties.deviceName);
        uint32_t queue_family_properties_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_properties_count, nullptr);
        queue_family_properties = std::vector<VkQueueFamilyProperties>(queue_family_properties_count);
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_properties_count, queue_family_properties.data());


    }
    bool PhysicalDevice::isPresentSupported(VkSurfaceKHR surface, uint32_t queueIndex) noexcept
    {
        VkBool32 present_supported{ VK_FALSE };

        if (surface != VK_NULL_HANDLE)
        {
            VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(m_handle, queueIndex, surface, &present_supported));
        }

        return present_supported;
    }
}