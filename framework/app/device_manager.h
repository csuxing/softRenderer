#ifndef _DEVICE_MANAGER_H_H_
#define _DEVICE_MANAGER_H_H_
#include <cstdint>
#include <vector>
#include <string>

namespace APP
{
    struct InstanceParameters
    {
        bool enableDebugRuntime = false;
        bool headlessDevice = false;

        std::vector<std::string> requiredVulkanInstanceExtensions;
        std::vector<std::string> requiredVulkanLayers;
        std::vector<std::string> optionalVulkanInstanceExtensions;
        std::vector<std::string> optionalVulkanLayers;
    };
    class DeviceManager
    {
    public:
        enum GraphicsAPI : uint8_t
        {
            kVk,
            kD3D11,
            kD3D12
        };
        DeviceManager* create(GraphicsAPI api);

    private:
        DeviceManager* createVK();
    };
}

#endif
