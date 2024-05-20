#include <volk.h>
#include "instance.h"

#include "error.h"
#include "helpers.h"
#include "../macro.h"
namespace RHI
{
    bool validate_layers(const std::vector<const char*>& required,
        const std::vector<VkLayerProperties>& available)
    {
        for (auto layer : required)
        {
            bool found = false;
            for (auto& available_layer : available)
            {
                if (strcmp(available_layer.layerName, layer) == 0)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                return false;
            }
        }

        return true;
    }

    bool enable_extension(const char* required_ext_name,
        const std::vector<VkExtensionProperties>& available_exts,
        std::vector<const char*>& enabled_extensions)
    {
        for (auto& avail_ext_it : available_exts)
        {
            if (strcmp(avail_ext_it.extensionName, required_ext_name) == 0)
            {
                auto it = std::find_if(enabled_extensions.begin(), enabled_extensions.end(),
                    [required_ext_name](const char* enabled_ext_name) {
                        return strcmp(enabled_ext_name, required_ext_name) == 0;
                    });
                if (it != enabled_extensions.end())
                {
                    // Extension is already enabled
                }
                else
                {
                    // LOGI("Extension {} found, enabling it", required_ext_name);
                    enabled_extensions.emplace_back(required_ext_name);
                }
                return true;
            }
        }

        // LOGI("Extension {} not found", required_ext_name);
        return false;
    }

    std::vector<const char*> get_optimal_validation_layers(const std::vector<VkLayerProperties>& supported_instance_layers)
    {
        std::vector<std::vector<const char*>> validation_layer_priority_list =
        {
            // The preferred validation layer is "VK_LAYER_KHRONOS_validation"
            {"VK_LAYER_KHRONOS_validation"},

            // Otherwise we fallback to using the LunarG meta layer
            {"VK_LAYER_LUNARG_standard_validation"},

            // Otherwise we attempt to enable the individual layers that compose the LunarG meta layer since it doesn't exist
            {
                "VK_LAYER_GOOGLE_threading",
                "VK_LAYER_LUNARG_parameter_validation",
                "VK_LAYER_LUNARG_object_tracker",
                "VK_LAYER_LUNARG_core_validation",
                "VK_LAYER_GOOGLE_unique_objects",
            },

            // Otherwise as a last resort we fallback to attempting to enable the LunarG core layer
            {"VK_LAYER_LUNARG_core_validation"} };

        for (auto& validation_layers : validation_layer_priority_list)
        {
            if (validate_layers(validation_layers, supported_instance_layers))
            {
                return validation_layers;
            }
        }
        // Else return nothing
        return {};
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

    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT /*type*/,
        uint64_t /*object*/, size_t /*location*/, int32_t /*message_code*/,
        const char* layer_prefix, const char* message, void* /*user_data*/)
    {
        if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
        {
            LOGE("{}: {}", layer_prefix, message);
        }
        else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
        {
            LOGW("{}: {}", layer_prefix, message);
        }
        else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
        {
            LOGW("{}: {}", layer_prefix, message);
        }
        else
        {
            LOGI("{}: {}", layer_prefix, message);
        }
        return VK_FALSE;
    }

    Instance::Instance(const std::string& application_name, const std::unordered_map<const char*, bool>& required_extensions, const std::vector<const char*>& required_validation_layers, bool headless, uint32_t api_version)
    {
        // instance extension
        uint32_t instance_extension_count{};
        VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, nullptr));

        std::vector<VkExtensionProperties> available_instance_extensions(instance_extension_count);
        VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, available_instance_extensions.data()));
        // instance layer
        uint32_t instance_layer_count;
        VK_CHECK(vkEnumerateInstanceLayerProperties(&instance_layer_count, nullptr));

        std::vector<VkLayerProperties> supported_validation_layers(instance_layer_count);
        VK_CHECK(vkEnumerateInstanceLayerProperties(&instance_layer_count, supported_validation_layers.data()));

        // add validator layer
        std::vector<const char*> requested_validation_layers(required_validation_layers);
        std::vector<const char*> optimal_validation_layers = get_optimal_validation_layers(supported_validation_layers);
        requested_validation_layers.insert(requested_validation_layers.end(), optimal_validation_layers.begin(), optimal_validation_layers.end());

        if (validate_layers(requested_validation_layers, supported_validation_layers))
        {
            for (const auto& layer : requested_validation_layers)
            {
                LOG_DEBUG("load " + layer);
            }
        }
        else
        {
            throw std::runtime_error("Required validation layers are missing.");
        }
        // add required extensions
        if (headless)
        {
            const bool has_headless_surface = enable_extension(VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME,
                available_instance_extensions, enabled_extensions);
            if (!has_headless_surface)
            {
                LOGW("{} is not available, disabling swapchain creation", VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME);
            }
        }
        else
        {
            enabled_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        }

        // VK_KHR_get_physical_device_properties2 is a prerequisite of VK_KHR_performance_query
        // which will be used for stats gathering where available.
        enable_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
            available_instance_extensions, enabled_extensions);
        
        auto extension_error = false;
        for (auto extension : required_extensions)
        {
            auto extension_name = extension.first;
            auto extension_is_optional = extension.second;
            if (!enable_extension(extension_name, available_instance_extensions, enabled_extensions))
            {
                if (extension_is_optional)
                {
                    LOGW("Optional instance extension {} not available, some features may be disabled", extension_name);
                }
                else
                {
                    LOGE("Required instance extension {} not available, cannot run", extension_name);
                    extension_error = true;
                }
                extension_error = extension_error || !extension_is_optional;
            }
        }

        VkApplicationInfo app_info{ VK_STRUCTURE_TYPE_APPLICATION_INFO };

        app_info.pApplicationName = application_name.c_str();
        app_info.applicationVersion = 0;
        app_info.pEngineName = "Jerry Engine";
        app_info.engineVersion = 0;
        app_info.apiVersion = api_version;

        VkInstanceCreateInfo instance_info = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };

        instance_info.pApplicationInfo = &app_info;

        instance_info.enabledExtensionCount = to_u32(enabled_extensions.size());
        instance_info.ppEnabledExtensionNames = enabled_extensions.data();

        instance_info.enabledLayerCount = to_u32(requested_validation_layers.size());
        instance_info.ppEnabledLayerNames = requested_validation_layers.data();
        VkDebugUtilsMessengerCreateInfoEXT debug_utils_create_info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        VkDebugReportCallbackCreateInfoEXT debug_report_create_info = { VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT };
        const bool has_debug_utils = enable_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
            available_instance_extensions, enabled_extensions);
        if (has_debug_utils)
        {
            debug_utils_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
            debug_utils_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            debug_utils_create_info.pfnUserCallback = debug_utils_messenger_callback;

            instance_info.pNext = &debug_utils_create_info;
        }
        else
        {
            debug_report_create_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
            debug_report_create_info.pfnCallback = debug_callback;

            instance_info.pNext = &debug_report_create_info;
        }
        VkResult result = vkCreateInstance(&instance_info, nullptr, &handle);

        if (result != VK_SUCCESS)
        {
            LOG_DEBUG("instance init failed!");
            return;
        }
        LOG_DEBUG("instance init successful!");

        volkLoadInstance(handle);
        query_gpus();
    }
    Instance::Instance(VkInstance instance)
    {

    }
    Instance::~Instance()
    {
    }
    void Instance::query_gpus()
    {
        // Querying valid physical devices on the machine
        uint32_t physical_device_count{ 0 };
        VK_CHECK(vkEnumeratePhysicalDevices(handle, &physical_device_count, nullptr));

        if (physical_device_count < 1)
        {
            throw std::runtime_error("Couldn't find a physical device that supports Vulkan.");
        }
         
        std::vector<VkPhysicalDevice> physical_devices;
        physical_devices.resize(physical_device_count);
        VK_CHECK(vkEnumeratePhysicalDevices(handle, &physical_device_count, physical_devices.data()));

        // Create gpus wrapper objects from the VkPhysicalDevice's
        for (auto& physical_device : physical_devices)
        {
            gpus.push_back(std::make_unique<PhysicalDevice>(getshardFromThis(), physical_device));
        }
    }
    PhysicalDevice& Instance::get_suitable_gpu(VkSurfaceKHR surface)
    {
        ASSERT(!gpus.empty() && "no physical devices were found on the system.");
        // find a discrete GPU
        for (auto& gpu : gpus)
        {
            // to do : (xingyuhang)
            if (gpu->getProperties().deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                size_t queueCount = gpu->getQueueFamilyProperties().size();
                for (uint32_t queueIndex = 0; queueIndex < queueCount; ++queueIndex)
                {
                    if (gpu->isPresentSupported(surface, queueIndex))
                    {
                        return *gpu;
                    }
                }
            }
        }
    }
    VkInstance Instance::get_handle() const
    {
        return handle;
    }
}