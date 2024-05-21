#ifndef _PHYSICAL_DEVICE_H_H_
#define _PHYSICAL_DEVICE_H_H_
#include <vector>
#include <memory>
#include "volk.h"
namespace RHI
{
    class Instance;
    class PhysicalDevice
    {
    public:
        PhysicalDevice(std::shared_ptr<Instance> instance, VkPhysicalDevice physical_device);

        PhysicalDevice(const PhysicalDevice&) = delete;

        PhysicalDevice(PhysicalDevice&&) = delete;

        PhysicalDevice& operator=(const PhysicalDevice&) = delete;

        PhysicalDevice& operator=(PhysicalDevice&&) = delete;

        VkPhysicalDeviceProperties getProperties() noexcept
        {
            return properties;
        }
        const std::vector<VkQueueFamilyProperties>& getQueueFamilyProperties() const noexcept
        {
            return queue_family_properties;
        }
        bool isPresentSupported(VkSurfaceKHR surface, uint32_t queueIndex) const noexcept;
        VkPhysicalDevice getHandle() const noexcept
        {
            return m_handle;
        }
        VkPhysicalDeviceFeatures getFeature() const noexcept
        {
            return m_requestFeatures;
        }
        std::shared_ptr<Instance> getInstance() const noexcept
        {
            return m_instance;
        }
    private:
        // Handle to the Vulkan instance
        std::shared_ptr<Instance> m_instance;
        // Handle to the Vulkan physical device
        VkPhysicalDevice m_handle{ VK_NULL_HANDLE };
        // The features that this GPU supports
        VkPhysicalDeviceFeatures features{};
        // The GPU properties
        VkPhysicalDeviceProperties properties;
        // The GPU memory
        VkPhysicalDeviceMemoryProperties memory_properties;
        // The GPU queue family properties
        std::vector<VkQueueFamilyProperties> queue_family_properties;

        VkPhysicalDeviceFeatures m_requestFeatures{};
    };
}

#endif
