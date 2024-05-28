#ifndef _INSTANCE_H__
#define _INSTANCE_H__
#include <string>
#include <unordered_map>
#include <memory>

#include <vulkan\vulkan.h>

#include "physical_device.h"
namespace RHI
{	
	class Instance : public std::enable_shared_from_this<Instance>
	{
	public:
		/**
		 * @brief Initializes the connection to Vulkan
		 * @param application_name The name of the application
		 * @param required_extensions The extensions requested to be enabled
		 * @param required_validation_layers The validation layers to be enabled
		 * @param headless Whether the application is requesting a headless setup or not
		 * @param api_version The Vulkan API version that the instance will be using
		 * @throws runtime_error if the required extensions and validation layers are not found
		 */
		Instance(const std::string& application_name,
			const std::unordered_map<const char*, bool>& required_extensions = {},
			const std::vector<const char*>& required_validation_layers = {},
			bool                                          headless = false,
			uint32_t                                      api_version = VK_API_VERSION_1_0);

        void init() noexcept
        {
            query_gpus();
        }

		/**
		 * @brief Queries the GPUs of a VkInstance that is already created
		 * @param instance A valid VkInstance
		 */
		Instance(VkInstance instance);

		Instance(const Instance&) = delete;

		Instance(Instance&&) = delete;

		~Instance();

		Instance& operator=(const Instance&) = delete;

		Instance& operator=(Instance&&) = delete;

		/**
		 * @brief Tries to find the first available discrete GPU that can render to the given surface
		 * @param surface to test against
		 * @returns A valid physical device
		 */
		PhysicalDevice& get_suitable_gpu(VkSurfaceKHR);

		/**
		 * @brief Tries to find the first available discrete GPU
		 * @returns A valid physical device
		 */
		PhysicalDevice& get_first_gpu();

		/**
		 * @brief Checks if the given extension is enabled in the VkInstance
		 * @param extension An extension to check
		 */
		bool is_enabled(const char* extension) const;

		VkInstance get_handle() const;

		const std::vector<const char*>& get_extensions();

		/**
		* @brief Returns a const ref to the properties of all requested layers in this instance
		* @returns The VkLayerProperties for all requested layers in this instance
		*/
		const std::vector<VkLayerProperties>& get_layer_properties();

		/**
		* @brief Finds layer properties for the layer with the given name
		* @param layerName The layer to search for
		* @param properties A reference to a VkLayerProperties struct to populate
		* @returns True if the layer was found and populated, false otherwise
		*/
		bool get_layer_properties(const char* layerName, VkLayerProperties& properties);

	private:
        std::shared_ptr<Instance> getshardFromThis()
        {
            return shared_from_this();
        }
        /**
        * @brief Queries the instance for the physical devices on the machine
        */
        void query_gpus();
		/**
		 * @brief The Vulkan instance
		 */
		VkInstance handle{ VK_NULL_HANDLE };

		/**
		 * @brief The enabled extensions
		 */
		std::vector<const char*> enabled_extensions;

#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
		/**
		 * @brief Debug utils messenger callback for VK_EXT_Debug_Utils
		 */
		VkDebugUtilsMessengerEXT debug_utils_messenger{ VK_NULL_HANDLE };

		/**
		 * @brief The debug report callback
		 */
		VkDebugReportCallbackEXT debug_report_callback{ VK_NULL_HANDLE };
#endif

		/**
		 * @brief The physical devices found on the machine
		 */
		std::vector<std::unique_ptr<PhysicalDevice>> gpus;
	};        // namespace Instance
}

#endif // _INSTANCE_H__
