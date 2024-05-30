#include "vk_device_manager.h"
#include "macro.h"
#include "core/error.h"
#include "core/helpers.h"
#include <GLFW/glfw3.h>
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
    }

    void VkDeviceManager::createInstance()
    {
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
        for (auto optExt : available_instance_extensions)
        {
            m_optionalExtension.instance.insert(optExt.extensionName);
        }
        uint32_t instance_layer_count;
        VK_CHECK(vkEnumerateInstanceLayerProperties(&instance_layer_count, nullptr));

        std::vector<VkLayerProperties> supported_validation_layers(instance_layer_count);
        VK_CHECK(vkEnumerateInstanceLayerProperties(&instance_layer_count, supported_validation_layers.data()));
        for (auto optLayer : supported_validation_layers)
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
    }

    void VkDeviceManager::createVKDevice()
    {
        queryGpu();
        createSurface();
        pickPhysicalDevice();
    }

    void VkDeviceManager::queryGpu()
    {
        uint32_t physical_device_count{ 0 };
        VK_CHECK(vkEnumeratePhysicalDevices(m_instance, &physical_device_count, nullptr));
        if (physical_device_count < 1)
        {
            throw std::runtime_error("Couldn't find a physical device that supports Vulkan.");
        }
        std::vector<VkPhysicalDevice> gpus;
        gpus.resize(physical_device_count);
        VK_CHECK(vkEnumeratePhysicalDevices(m_instance, &physical_device_count, gpus.data()));
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
    }

    void VkDeviceManager::createLogicalDevice()
    {
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
}