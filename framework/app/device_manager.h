#ifndef _DEVICE_MANAGER_H_H_
#define _DEVICE_MANAGER_H_H_
#include <cstdint>
#include <vector>
#include <string>
#include <functional>
struct GLFWwindow;
namespace APP
{
    enum ColorFromat : uint8_t
    {
        SRGBA8_UNORM
    };

    struct InstanceParameters
    {
        bool enableDebugRuntime = false;
        bool headlessDevice = false;

        std::vector<std::string> requiredVulkanInstanceExtensions;
        std::vector<std::string> requiredVulkanLayers;
        std::vector<std::string> optionalVulkanInstanceExtensions;
        std::vector<std::string> optionalVulkanLayers;
    };

    struct DeviceCreationParameters
    {
        InstanceParameters m_instanceParameters;

        GLFWwindow* m_window{ nullptr };
        std::string m_engineName{"Jerry"};
        uint32_t m_apiVersion{};
        bool startMaximized = false;
        bool startFullscreen = false;
        bool allowModeSwitch = true;
        int windowPosX = -1;            // -1 means use default placement
        int windowPosY = -1;
        uint32_t backBufferWidth = 1280;
        uint32_t backBufferHeight = 720;
        uint32_t refreshRate = 0;
        uint32_t swapChainBufferCount = 3;
        ColorFromat swapChainFormat = ColorFromat::SRGBA8_UNORM;
        uint32_t swapChainSampleCount = 1;
        uint32_t swapChainSampleQuality = 0;
        uint32_t maxFramesInFlight = 2;
        bool enableNvrhiValidationLayer = false;
        bool vsyncEnabled = false;
        bool enableRayTracingExtensions = false; // for vulkan
        bool enableComputeQueue = false;
        bool enableCopyQueue = false;
        int adapterIndex = -1;
        bool enablePerMonitorDPI = false;

        std::vector<std::string> requiredVulkanDeviceExtensions;
        std::vector<std::string> optionalVulkanDeviceExtensions;
        std::vector<size_t> ignoredVulkanValidationMessageLocations;
        // std::function<void(VkDeviceCreateInfo&)> deviceCreateInfoCallback;
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
        static DeviceManager* create(GraphicsAPI api);

        virtual ~DeviceManager(){};
        virtual void createDevice(DeviceCreationParameters parameters) = 0;

    private:
        static DeviceManager* createVK();
    };
}

#endif
