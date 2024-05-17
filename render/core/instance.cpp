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
                LOG_DEBUG("load " + layer + " failed");
            }
        }
        else
        {
            throw std::runtime_error("Required validation layers are missing.");
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

        LOG_DEBUG("instance init successful!");
    }
    Instance::Instance(VkInstance instance)
    {

    }
    Instance::~Instance()
    {
    }
}
