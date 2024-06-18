#include "vk_device_manager.h"
// c++ stl
#include <set>
#include <map>
// thirdParty
#include <GLFW/glfw3.h>
// current project other lib
#include "core/error.h"
#include "core/helpers.h"
#include "core/command_pool.h"
#include "core/fence_pool.h"
#include "core/command_buffer.h"
// current lib
#include "macro.h"

namespace APP
{
    static std::vector<const char*> stringSetToVector(const std::unordered_set<std::string>& set)
    {
        std::vector<const char*> ret;
        for (const auto& s : set)
        {
            ret.push_back(s.c_str());
        }

        return ret;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void* user_data)
    {
        // Log debug messge
        if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            LOGW("{} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage);
        }
        else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            LOGE("{} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage);
        }
        return VK_FALSE;
    }

    VkDeviceManager::~VkDeviceManager()
    {
    }

    void VkDeviceManager::createDevice(DeviceCreationParameters parameters)
    {
        m_deviceCreationParameters = parameters;
        for (auto ext : m_deviceCreationParameters.m_instanceParameters.requiredVulkanInstanceExtensions)
        {
            m_requiredExtension.instance.insert(ext);
        }

        VkResult result = volkInitialize();
        if (result != VK_SUCCESS)
        {
            LOG_FATAL("volk initialize failed! ");
        }
        createInstance();
        createVKDevice();
        createSwapchain();
        m_commandPool = new RHI::CommandPool(this, m_GraphicsQueueFamily);
        m_fencePool = new RHI::FencePool(this);
        createPipelineCache();
    }

    RHI::CommandBuffer& VkDeviceManager::requestCommandBuffer()
    {
        return m_commandPool->requestCommandBuffer();
    }

    void VkDeviceManager::createInstance()
    {
        if (m_instanceCreated)
        {
            return;
        }

        uint32_t apiVersion{};
        vkEnumerateInstanceVersion(&apiVersion);
        m_deviceCreationParameters.m_apiVersion = VK_API_VERSION_1_1;
        if (m_deviceCreationParameters.m_instanceParameters.enableDebugRuntime)
        {
            m_requiredExtension.instance.insert("VK_EXT_debug_report");
            m_requiredExtension.instance.insert("VK_EXT_debug_utils");
            m_requiredExtension.layers.insert("VK_LAYER_KHRONOS_validation");
        }

        // instance extension
        uint32_t instance_extension_count{};
        VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, nullptr));
        std::vector<VkExtensionProperties> available_instance_extensions(instance_extension_count);
        VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, available_instance_extensions.data()));
        for (const auto& optExt : available_instance_extensions)
        {
            m_optionalExtension.instance.insert(optExt.extensionName);
        }
        uint32_t instance_layer_count;
        VK_CHECK(vkEnumerateInstanceLayerProperties(&instance_layer_count, nullptr));

        std::vector<VkLayerProperties> supported_validation_layers(instance_layer_count);
        VK_CHECK(vkEnumerateInstanceLayerProperties(&instance_layer_count, supported_validation_layers.data()));
        for (const auto& optLayer : supported_validation_layers)
        {
            m_optionalExtension.layers.insert(optLayer.layerName);
        }
        if (!validateInstanceExtensionAndLayer())
        {
            throw std::runtime_error("lose of instance extension or layers!");
        }

        VkApplicationInfo app_info{ VK_STRUCTURE_TYPE_APPLICATION_INFO };

        app_info.pApplicationName = m_deviceCreationParameters.m_engineName.c_str();
        app_info.applicationVersion = 0;
        app_info.pEngineName = "Jerry Engine";
        app_info.engineVersion = 0;
        app_info.apiVersion = m_deviceCreationParameters.m_apiVersion;

        auto enabledExtensions = stringSetToVector(m_requiredExtension.instance);
        auto enabledLayers = stringSetToVector(m_requiredExtension.layers);

        VkInstanceCreateInfo instance_info = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        instance_info.pApplicationInfo = &app_info;
        instance_info.enabledExtensionCount = to_u32(enabledExtensions.size());
        instance_info.ppEnabledExtensionNames = enabledExtensions.data();
        instance_info.enabledLayerCount = to_u32(enabledLayers.size());
        instance_info.ppEnabledLayerNames = enabledLayers.data();

        if (m_deviceCreationParameters.m_instanceParameters.enableDebugRuntime)
        {
            VkDebugUtilsMessengerCreateInfoEXT debug_utils_create_info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
            debug_utils_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
            debug_utils_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            debug_utils_create_info.pfnUserCallback = debug_utils_messenger_callback;
            instance_info.pNext = &debug_utils_create_info;
        }
        VK_CHECK(vkCreateInstance(&instance_info, nullptr, &m_instance));
        volkLoadInstance(m_instance);
        m_instanceCreated = true;
    }

    void VkDeviceManager::createVKDevice()
    {
        queryGpu();
        createSurface();
        pickPhysicalDevice();
        findQueueFamilies();
        createLogicalDevice();
        initVma();
    }

    void VkDeviceManager::createSwapchain()
    {
        // todo(xingyuhang) : destory old swapchain
        if (m_swapchainInfo.handle != VK_NULL_HANDLE)
        {
            uint32_t imageCount = to_u32(m_swapchainInfo.imageViews.size());
            for (uint32_t i = 0; i < imageCount; i++)
            {
                vkDestroyImageView(m_logicalDevice, m_swapchainInfo.imageViews[i], nullptr);
            }
            vkDestroySwapchainKHR(m_logicalDevice, m_swapchainInfo.handle, nullptr);
        }
        // todo(xingyuhang) : select format and present mode more perfect
        VkPhysicalDevice physicalDeviceHnadle = getCurrentUseGpu().handle;
        uint32_t surfaceFormatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDeviceHnadle, m_surface, &surfaceFormatCount, nullptr);
        std::vector<VkSurfaceFormatKHR> formatKHRs;
        formatKHRs.resize(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDeviceHnadle, m_surface, &surfaceFormatCount, formatKHRs.data());
        VkSurfaceFormatKHR temp = m_swapchainInfo.format;
        if (std::find_if(formatKHRs.begin(), formatKHRs.end(), [temp](VkSurfaceFormatKHR input) { 
            if (input.format == temp.format && input.colorSpace == temp.colorSpace) 
            {
                return true;
            }
            else
            {
                return false;
            }
            }) == formatKHRs.end())
        {
            m_swapchainInfo.format = *formatKHRs.begin();
        }

        uint32_t surfacePresentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDeviceHnadle, m_surface, &surfacePresentModeCount, nullptr);
        std::vector<VkPresentModeKHR> modes;
        modes.resize(surfacePresentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDeviceHnadle, m_surface, &surfaceFormatCount, modes.data());

        if (std::find(modes.begin(), modes.end(), m_swapchainInfo.presentMode) == modes.end())
        {
            m_swapchainInfo.presentMode = *modes.begin();
        }

        VkSurfaceCapabilitiesKHR capablities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDeviceHnadle, m_surface, &capablities);

        VkSwapchainCreateInfoKHR swapchainCreateInfo;
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.pNext = nullptr;
        swapchainCreateInfo.flags = 0;
        swapchainCreateInfo.surface = m_surface;
        swapchainCreateInfo.minImageCount = capablities.minImageCount + 1;
        swapchainCreateInfo.imageFormat = m_swapchainInfo.format.format;
        swapchainCreateInfo.imageColorSpace = m_swapchainInfo.format.colorSpace;
        swapchainCreateInfo.imageExtent = capablities.currentExtent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;
        swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = m_swapchainInfo.presentMode;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = m_swapchainInfo.handle;
        VK_CHECK(vkCreateSwapchainKHR(m_logicalDevice, &swapchainCreateInfo, nullptr, &m_swapchainInfo.handle));
        m_swapchainInfo.extent = capablities.currentExtent;
        uint32_t imageCount{};
        vkGetSwapchainImagesKHR(m_logicalDevice, m_swapchainInfo.handle, &imageCount, nullptr);
        m_swapchainInfo.images.resize(imageCount);
        vkGetSwapchainImagesKHR(m_logicalDevice, m_swapchainInfo.handle, &imageCount, m_swapchainInfo.images.data());
        // create image views
        m_swapchainInfo.imageViews.resize(imageCount);
        for (uint32_t i = 0; i < imageCount; i++)
        {
            VkImageViewCreateInfo imageViewCreateInfo{};
            imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCreateInfo.pNext = nullptr;
            imageViewCreateInfo.flags = 0;
            imageViewCreateInfo.image = m_swapchainInfo.images[i];
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewCreateInfo.format = m_swapchainInfo.format.format;
            imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
            imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
            imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
            imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewCreateInfo.subresourceRange.layerCount = 1;
            imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            imageViewCreateInfo.subresourceRange.levelCount = 1;
            imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
            VK_CHECK(vkCreateImageView(m_logicalDevice, &imageViewCreateInfo, nullptr, &m_swapchainInfo.imageViews[i]))
        }
    }

    void VkDeviceManager::createPipelineCache()
    {
        VkPipelineCacheCreateInfo pipelineCacheCi{};
        pipelineCacheCi.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        auto res = vkCreatePipelineCache(m_logicalDevice, &pipelineCacheCi, nullptr, &m_pipelineCache);
        if (res != VK_SUCCESS)
        {
            LOG_ERROR("create pipeline cache failed!");
        }
    }

    void VkDeviceManager::queryGpu()
    {
        uint32_t physical_device_count{ 0 };
        VK_CHECK(vkEnumeratePhysicalDevices(m_instance, &physical_device_count, nullptr))
        if (physical_device_count < 1)
        {
            throw std::runtime_error("Couldn't find a physical device that supports Vulkan.");
        }
        std::vector<VkPhysicalDevice> gpus;
        gpus.resize(physical_device_count);
        VK_CHECK(vkEnumeratePhysicalDevices(m_instance, &physical_device_count, gpus.data()))
        m_gpus.resize(physical_device_count);
        LOG_INFO("find {} physical device", physical_device_count);
        for (size_t i = 0; i < physical_device_count; ++i)
        {
            auto& gpuInfo = m_gpus[i];
            gpuInfo.handle = gpus[i];
            vkGetPhysicalDeviceFeatures(gpuInfo.handle, &gpuInfo.features);
            vkGetPhysicalDeviceProperties(gpuInfo.handle, &gpuInfo.properties);
            vkGetPhysicalDeviceMemoryProperties(gpuInfo.handle, &gpuInfo.memoryProperties);
            LOG_INFO("{} gpu : {}", i, gpuInfo.properties.deviceName);
            uint32_t queueFamilyPropertiesCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(gpuInfo.handle, &queueFamilyPropertiesCount, nullptr);
            gpuInfo.queueFamilyProperties.resize(queueFamilyPropertiesCount);
            vkGetPhysicalDeviceQueueFamilyProperties(gpuInfo.handle, &queueFamilyPropertiesCount, gpuInfo.queueFamilyProperties.data());
        }
    }

    void VkDeviceManager::createSurface()
    {
        ASSERT(m_deviceCreationParameters.m_window && "window nust not be nullptr");
        if (m_instance == VK_NULL_HANDLE)
        {
            LOG_DEBUG("instance iis nullptr!");
            return;
        }

        VkResult errCode = glfwCreateWindowSurface(m_instance, m_deviceCreationParameters.m_window, NULL, &m_surface);
        if (errCode != VK_SUCCESS)
        {
            LOG_DEBUG("create surface failed!");
            return;
        }
    }

    void VkDeviceManager::pickPhysicalDevice()
    {
        ASSERT(!m_gpus.empty() && "no physical devices were found on the system.");
        ASSERT(m_surface && "surface should not be nullptr!");
        // find a discrete GPU
        uint32_t physicalDeviceCount = to_u32(m_gpus.size());
        for (uint32_t i = 0; i < physicalDeviceCount; ++i)
        {
            auto& gpu = m_gpus[i];
            if (gpu.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                size_t queueCount = gpu.queueFamilyProperties.size();
                for (uint32_t queueIndex = 0; queueIndex < queueCount; ++queueIndex)
                {
                    VkBool32 present_supported{ VK_FALSE };
                    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu.handle, queueIndex, m_surface, &present_supported));
                    if (present_supported)
                    {
                        m_currentGpuIndex = i;
                        LOG_INFO("select physical device gpu : {}", gpu.properties.deviceName);
                        break;
                    }
                }
            }
        }
        uint32_t deviceExtensionCount{};
        vkEnumerateDeviceExtensionProperties(getCurrentUseGpu().handle, nullptr, &deviceExtensionCount, nullptr);
        std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);
        vkEnumerateDeviceExtensionProperties(getCurrentUseGpu().handle, nullptr, &deviceExtensionCount, deviceExtensions.data());

        for (const auto& ext : deviceExtensions)
        {
            m_optionalExtension.device.insert(ext.extensionName);
        }

        for (const auto& ext : m_deviceCreationParameters.requiredVulkanDeviceExtensions)
        {
            m_requiredExtension.device.insert(ext);
        }

        for (const auto& needNext : m_requiredExtension.device)
        {
            if (m_optionalExtension.device.find(needNext) != m_optionalExtension.device.end())
            {
                LOG_INFO("required device extension : {}", needNext);
            }
            else
            {
                LOG_ERROR("device extension : {} is not find!", needNext);
            }
        }
    }

    VkFence VkDeviceManager::requestFence()
    {
        return m_fencePool->requestFence();
    }

    VkResult VkDeviceManager::waitForFences(std::vector<VkFence> fences)
    {
        return vkWaitForFences(m_logicalDevice, to_u32(fences.size()), fences.data(), VK_TRUE, UINT64_MAX);
    }

    void VkDeviceManager::findQueueFamilies()
    {
        const GpuInfo& GPU = getCurrentUseGpu();
        auto& props = GPU.queueFamilyProperties;
        size_t familyCount = props.size();
        // fix me : queue family is different between them
        for (int i = 0; i < familyCount; ++i)
        {
            auto& queueFamilyProperty = props[i];

            if (queueFamilyProperty.queueCount < 1)
            {
                continue;
            }

            if (m_GraphicsQueueFamily == -1)
            {
                if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    m_GraphicsQueueFamily = i;
                }
            }

            if (m_ComputeQueueFamily == -1)
            {
                if (!(queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
                    queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT)
                {
                    m_ComputeQueueFamily = i;
                }
            }
            
            if (m_TransferQueueFamily == -1)
            {
                if (!(queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
                    !(queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT) &&
                    (queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT))
                {
                    m_TransferQueueFamily = i;
                }
            }

            if (m_PresentQueueFamily == -1)
            {
                if (glfwGetPhysicalDevicePresentationSupport(m_instance, GPU.handle, i))
                {
                    m_PresentQueueFamily = i;
                }
            }
        }

        ASSERT(m_GraphicsQueueFamily != -1);
        ASSERT(m_ComputeQueueFamily != -1);
        ASSERT(m_TransferQueueFamily != -1);
        ASSERT(m_PresentQueueFamily != -1);

        LOG_INFO("Graphics queue family index : {}", m_GraphicsQueueFamily);
        LOG_INFO("Compute queue family index  : {}", m_ComputeQueueFamily);
        LOG_INFO("Transfer queue family index : {}", m_TransferQueueFamily);
        LOG_INFO("Present queue family index  : {}", m_PresentQueueFamily);
    }

    VkResult VkDeviceManager::submit(VkQueue queue, const RHI::CommandBuffer& cmdBuffer, VkFence fence)
    {
        VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.commandBufferCount = 1;
        auto handle = cmdBuffer.getHandle();
        submitInfo.pCommandBuffers = &handle;
        return vkQueueSubmit(queue, 1, &submitInfo, fence);
    }

    void VkDeviceManager::createLogicalDevice()
    {
        VkDeviceCreateInfo deviceCrateInfo{};
        deviceCrateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCrateInfo.flags = 0;
        deviceCrateInfo.pNext = nullptr;

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<int> queueFamilyIndexs;
        queueFamilyIndexs.insert(m_GraphicsQueueFamily);
        queueFamilyIndexs.insert(m_ComputeQueueFamily);
        queueFamilyIndexs.insert(m_TransferQueueFamily);
        queueFamilyIndexs.insert(m_PresentQueueFamily);

        std::map<int, std::vector<float>> queuePriorities;
        for (int index : queueFamilyIndexs)
        {
            auto& properties = getCurrentUseGpu().queueFamilyProperties[index];
            queueCreateInfos.push_back({});
            auto& queueCreateInfo = queueCreateInfos.back();
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.flags = 0;
            queueCreateInfo.pNext = nullptr;
            queueCreateInfo.queueFamilyIndex = index;
            queueCreateInfo.queueCount = properties.queueCount;
            std::vector<float> priorities;
            for (uint32_t i = 0; i < properties.queueCount; ++i)
            {
                priorities.push_back(1.f);
            }
            queuePriorities.insert({ index, priorities });
            queueCreateInfo.pQueuePriorities = queuePriorities[index].data();
        }

        auto extensions = stringSetToVector(m_requiredExtension.device);

        deviceCrateInfo.queueCreateInfoCount = to_u32(queuePriorities.size());
        deviceCrateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCrateInfo.enabledLayerCount = 0;
        deviceCrateInfo.ppEnabledLayerNames = nullptr;
        deviceCrateInfo.enabledExtensionCount = to_u32(extensions.size());
        deviceCrateInfo.ppEnabledExtensionNames = extensions.data();
        deviceCrateInfo.pEnabledFeatures = &m_physicalDevicefeatures;
        VK_CHECK(vkCreateDevice(getCurrentUseGpu().handle, &deviceCrateInfo, nullptr, &m_logicalDevice));
        LOG_INFO("create logical device successful!");

        // get vkqueue handle
        vkGetDeviceQueue(m_logicalDevice, m_GraphicsQueueFamily, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_logicalDevice, m_ComputeQueueFamily, 0, &m_ComputeQueue);
        vkGetDeviceQueue(m_logicalDevice, m_TransferQueueFamily, 0, &m_TransferQueue);
        vkGetDeviceQueue(m_logicalDevice, m_PresentQueueFamily, 0, &m_PresentQueue);

        ASSERT(m_GraphicsQueue != VK_NULL_HANDLE);
        ASSERT(m_ComputeQueue != VK_NULL_HANDLE);
        ASSERT(m_TransferQueue != VK_NULL_HANDLE);
        ASSERT(m_PresentQueue != VK_NULL_HANDLE);
    }

    bool VkDeviceManager::validateInstanceExtensionAndLayer()
    {
        for (const auto& ext : m_requiredExtension.instance)
        {
            if (m_optionalExtension.instance.find(ext) != m_optionalExtension.instance.end())
            {
                LOG_INFO("require instance extension : {}", ext);
            }
            else
            {
                LOG_INFO("instance extension : {} not find in machine!");
                return false;
            }
        }

        for (const auto& ext : m_requiredExtension.layers)
        {
            if (m_optionalExtension.layers.find(ext) != m_optionalExtension.layers.end())
            {
                LOG_INFO("require instance layer : {}", ext);
            }
            else
            {
                LOG_INFO("instance layer : {} not find in machine!");
                return false;
            }
        }

        return true;
    }
    void VkDeviceManager::initVma()
    {
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
        allocator_info.physicalDevice = getCurrentUseGpu().handle;
        allocator_info.device = m_logicalDevice;
        allocator_info.instance = m_instance;

        allocator_info.pVulkanFunctions = &vma_vulkan_func;
        VkResult result = vmaCreateAllocator(&allocator_info, &m_memoryAllocator);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error{ "Cannot create allocator" };
        }
    }
}