#include "device.h"
#include "physical_device.h"
#include "helpers.h"
#include "instance.h"
#include "macro.h"
#include "error.h"
namespace RHI
{
    Device::Device(PhysicalDevice& gpu, VkSurfaceKHR surface, std::unordered_map<const char*, bool> requested_extensions)
        : VulkanResource{ VK_NULL_HANDLE, this },
        m_gpu(gpu)
    {
        LOGI("Selected GPU: {}", gpu.getProperties().deviceName);
        // prepare the device queues
        uint32_t                             queue_family_properties_count = to_u32(gpu.getQueueFamilyProperties().size());
        std::vector<VkDeviceQueueCreateInfo> queue_create_infos(queue_family_properties_count, { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO });
        std::vector<std::vector<float>>      queue_priorities(queue_family_properties_count);
        for (uint32_t queue_family_index = 0U; queue_family_index < queue_family_properties_count; ++queue_family_index)
        {
            const VkQueueFamilyProperties& queue_family_property = gpu.getQueueFamilyProperties()[queue_family_index];

            // todo(jerry) : do not use priority queue
            if (false)
            {
                uint32_t graphics_queue_family = get_queue_family_index(VK_QUEUE_GRAPHICS_BIT);
                if (graphics_queue_family == queue_family_index)
                {
                    queue_priorities[queue_family_index].reserve(queue_family_property.queueCount);
                    queue_priorities[queue_family_index].push_back(1.0f);
                    for (uint32_t i = 1; i < queue_family_property.queueCount; i++)
                    {
                        queue_priorities[queue_family_index].push_back(0.5f);
                    }
                }
                else
                {
                    queue_priorities[queue_family_index].resize(queue_family_property.queueCount, 0.5f);
                }
            }
            else
            {
                queue_priorities[queue_family_index].resize(queue_family_property.queueCount, 0.5f);
            }

            VkDeviceQueueCreateInfo& queue_create_info = queue_create_infos[queue_family_index];

            queue_create_info.queueFamilyIndex = queue_family_index;
            queue_create_info.queueCount = queue_family_property.queueCount;
            queue_create_info.pQueuePriorities = queue_priorities[queue_family_index].data();
        }

        uint32_t device_extension_count;
        VK_CHECK(vkEnumerateDeviceExtensionProperties(m_gpu.getHandle(), nullptr, &device_extension_count, nullptr));
        m_deviceExtensions = std::vector<VkExtensionProperties>(device_extension_count);
        VK_CHECK(vkEnumerateDeviceExtensionProperties(m_gpu.getHandle(), nullptr, &device_extension_count, m_deviceExtensions.data()));

        // display supported extensions
        if (!m_deviceExtensions.empty())
        {
            for (auto& extension : m_deviceExtensions)
            {
                LOGD("  \t{}", extension.extensionName);
            }
        }

        // enabled device extensions
        for (auto& extension : requested_extensions)
        {
            if (isExtensionSupported(extension.first))
            {
                m_enabledExtensions.push_back(extension.first);
            }
        }
        
        VkDeviceCreateInfo createInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        createInfo.pNext = nullptr;
        createInfo.pQueueCreateInfos = queue_create_infos.data();
        createInfo.queueCreateInfoCount = to_u32(queue_create_infos.size());
        createInfo.enabledExtensionCount = to_u32(m_enabledExtensions.size());
        createInfo.ppEnabledExtensionNames = m_enabledExtensions.data();

        const auto requested_gpu_features = m_gpu.getFeature();
        createInfo.pEnabledFeatures = &requested_gpu_features;

        VkResult result = vkCreateDevice(m_gpu.getHandle(), &createInfo, nullptr, &m_handle);
        if (VK_SUCCESS != result)
        {
            throw std::runtime_error("Cannot create device");
        }
        m_queues.resize(queue_family_properties_count);
        for (uint32_t queueFamilyIndex = 0u; queueFamilyIndex < queue_family_properties_count; ++queueFamilyIndex)
        {
            const VkQueueFamilyProperties& queueFasmilyProperty = m_gpu.getQueueFamilyProperties()[queueFamilyIndex];
            VkBool32 present_supported = m_gpu.isPresentSupported(surface, queueFamilyIndex);
            for (uint32_t queue_index = 0U; queue_index < queueFasmilyProperty.queueCount; ++queue_index)
            {
                m_queues[queueFamilyIndex].emplace_back(*this, queueFamilyIndex, queueFasmilyProperty, present_supported, queue_index);
            }
        }
        // init vma
        VmaVulkanFunctions vma_vulkan_func{};
        vma_vulkan_func.vkAllocateMemory = vkAllocateMemory;
        vma_vulkan_func.vkBindBufferMemory = vkBindBufferMemory;
        vma_vulkan_func.vkBindImageMemory = vkBindImageMemory;
        vma_vulkan_func.vkCreateBuffer = vkCreateBuffer;
        vma_vulkan_func.vkCreateImage = vkCreateImage;
        vma_vulkan_func.vkDestroyBuffer = vkDestroyBuffer;
        vma_vulkan_func.vkDestroyImage = vkDestroyImage;
        vma_vulkan_func.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
        vma_vulkan_func.vkFreeMemory = vkFreeMemory;
        vma_vulkan_func.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
        vma_vulkan_func.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
        vma_vulkan_func.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
        vma_vulkan_func.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
        vma_vulkan_func.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
        vma_vulkan_func.vkMapMemory = vkMapMemory;
        vma_vulkan_func.vkUnmapMemory = vkUnmapMemory;
        vma_vulkan_func.vkCmdCopyBuffer = vkCmdCopyBuffer;

        vma_vulkan_func.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        vma_vulkan_func.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocator_info{};
        allocator_info.physicalDevice = m_gpu.getHandle();
        allocator_info.device = m_handle;
        allocator_info.instance = m_gpu.getInstance()->get_handle();

        allocator_info.pVulkanFunctions = &vma_vulkan_func;
        result = vmaCreateAllocator(&allocator_info, &m_memoryAllocator);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error{ "Cannot create allocator" };
        }
        // create commandPool && fencePool
        //m_commandPool = std::make_unique<CommandPool>(*this, getQueueByFlags(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, 0).getFamilyIndex());
        //m_fencePool   = std::make_unique<FencePool>(*this);
        LOG_DEBUG("Device init successful!!!");
}

    uint32_t RHI::Device::get_queue_family_index(VkQueueFlagBits queue_flag)
    {
        const auto& queue_family_properties = m_gpu.getQueueFamilyProperties();

        // Dedicated queue for compute
        // Try to find a queue family index that supports compute but not graphics
        if (queue_flag & VK_QUEUE_COMPUTE_BIT)
        {
            for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties.size()); i++)
            {
                if ((queue_family_properties[i].queueFlags & queue_flag) && !(queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
                {
                    return i;
                    break;
                }
            }
        }

        // Dedicated queue for transfer
        // Try to find a queue family index that supports transfer but not graphics and compute
        if (queue_flag & VK_QUEUE_TRANSFER_BIT)
        {
            for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties.size()); i++)
            {
                if ((queue_family_properties[i].queueFlags & queue_flag) && !(queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && !(queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT))
                {
                    return i;
                    break;
                }
            }
        }

        // For other queue types or if no separate compute queue is present, return the first one to support the requested flags
        for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties.size()); i++)
        {
            if (queue_family_properties[i].queueFlags & queue_flag)
            {
                return i;
                break;
            }
        }

        throw std::runtime_error("Could not find a matching queue family index");
    }

    bool Device::isExtensionSupported(const std::string& requested_extension) const
    {
        return std::find_if(m_deviceExtensions.begin(), m_deviceExtensions.end(), [requested_extension](auto& deviceExtension)
            { return std::strcmp(deviceExtension.extensionName, requested_extension.c_str()) == 0; }) != m_deviceExtensions.end();
    }
    const Queue& Device::getQueueByFlags(VkQueueFlags queue_flags, uint32_t queue_index) const
    {
        for (uint32_t queue_family_index = 0U; queue_family_index < m_queues.size(); ++queue_family_index)
        {
            Queue const& first_queue = m_queues[queue_family_index][0];

            VkQueueFlags queue_flags = first_queue.getProperties().queueFlags;
            uint32_t     queue_count = first_queue.getProperties().queueCount;

            if (((queue_flags & queue_flags) == queue_flags) && queue_index < queue_count)
            {
                return m_queues[queue_family_index][queue_index];
            }
        }

        throw std::runtime_error("Queue not found");
    }
}