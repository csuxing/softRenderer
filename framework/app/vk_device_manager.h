#ifndef _VK_DEVICE_MANAGER_H_H_
#define _VK_DEVICE_MANAGER_H_H_
#include "device_manager.h"
#include <functional>
#include <volk.h>
namespace APP
{
    enum ColorFromat : uint8_t
    {
        SRGBA8_UNORM
    };
    struct DeviceCreationParameters
    {
        InstanceParameters m_instanceParameters;

        bool startMaximized             = false;
        bool startFullscreen            = false;
        bool allowModeSwitch            = true;
        int windowPosX                  = -1;            // -1 means use default placement
        int windowPosY                  = -1;
        uint32_t backBufferWidth        = 1280;
        uint32_t backBufferHeight       = 720;
        uint32_t refreshRate            = 0;
        uint32_t swapChainBufferCount   = 3;
        ColorFromat swapChainFormat     = ColorFromat::SRGBA8_UNORM;
        uint32_t swapChainSampleCount   = 1;
        uint32_t swapChainSampleQuality = 0;
        uint32_t maxFramesInFlight      = 2;
        bool enableNvrhiValidationLayer = false;
        bool vsyncEnabled               = false;
        bool enableRayTracingExtensions = false; // for vulkan
        bool enableComputeQueue         = false;
        bool enableCopyQueue            = false;
        int adapterIndex                = -1;
        bool enablePerMonitorDPI        = false;

        std::vector<std::string> requiredVulkanDeviceExtensions;
        std::vector<std::string> optionalVulkanDeviceExtensions;
        std::vector<size_t> ignoredVulkanValidationMessageLocations;
        std::function<void(VkDeviceCreateInfo&)> deviceCreateInfoCallback;
    };
    class VkDeviceManager : public DeviceManager
    {
    public:


    };
}

#endif 
