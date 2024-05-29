#ifndef _VK_DEVICE_MANAGER_H_H_
#define _VK_DEVICE_MANAGER_H_H_
#include "device_manager.h"
#include <unordered_set>
#include <vector>
#include <volk.h>
namespace APP
{
    struct VulkanExtensionSet
    {
        std::unordered_set<std::string> instance;
        std::unordered_set<std::string> layers;
        std::unordered_set<std::string> device;
    };
    struct GpuInfo
    {
        VkPhysicalDevice                        handle;
        VkPhysicalDeviceFeatures                features{};
        VkPhysicalDeviceProperties              properties;
        VkPhysicalDeviceMemoryProperties        memoryProperties;
        std::vector<VkQueueFamilyProperties>    queueFamilyProperties;
    };
    class VkDeviceManager : public DeviceManager
    {
    public:
        void createDevice(DeviceCreationParameters parameters) override;
    protected:
        void createInstance();
        void createVKDevice();

        void queryGpu();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
    private:
        bool validateInstanceExtensionAndLayer();

        DeviceCreationParameters        m_deviceCreationParameters{};

        bool                            m_instanceCreated{ false };
        VulkanExtensionSet              m_requiredExtension;
        VulkanExtensionSet              m_optionalExtension; // instance extensions && layer && device extensions in current PC
        std::vector<GpuInfo>            m_gpus;

        VkInstance                      m_instance{ VK_NULL_HANDLE };
        VkDevice                        m_VulkanDevice{VK_NULL_HANDLE};
        VkSurfaceKHR                    m_surface;
        uint32_t                        m_currentGpuIndex{};

        VkQueue                         m_GraphicsQueue{VK_NULL_HANDLE};
        VkQueue                         m_ComputeQueue{VK_NULL_HANDLE};
        VkQueue                         m_TransferQueue{VK_NULL_HANDLE};
        VkQueue                         m_PresentQueue{VK_NULL_HANDLE};
    };
}

#endif 
