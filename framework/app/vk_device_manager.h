#ifndef _VK_DEVICE_MANAGER_H_H_
#define _VK_DEVICE_MANAGER_H_H_
#include "device_manager.h"
#include <unordered_set>
#include <vector>
#include <volk.h>
#include "vk_mem_alloc.h"
namespace RHI
{
    class CommandPool;
    class CommandBuffer;
    class FencePool;
}
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
        VkPhysicalDeviceFeatures                features;
        VkPhysicalDeviceProperties              properties;
        VkPhysicalDeviceMemoryProperties        memoryProperties;
        std::vector<VkQueueFamilyProperties>    queueFamilyProperties;
    };
    struct SwapchainInfo
    {
        VkSurfaceFormatKHR          format{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        VkPresentModeKHR            presentMode{ VK_PRESENT_MODE_MAILBOX_KHR };
        VkSwapchainKHR              handle{ VK_NULL_HANDLE };
        VkExtent2D                  extent{};
        std::vector<VkImage>        images;
        std::vector<VkImageView>    imageViews;
    };
    class VkDeviceManager : public DeviceManager
    {
    public:
        virtual ~VkDeviceManager() override;
        void createDevice(DeviceCreationParameters parameters) override;

        VkInstance getInstance() const { return m_instance; }
        VkDevice   getDevice() const { return  m_logicalDevice; }
        VkSurfaceKHR getSurface() const { return m_surface; }
        SwapchainInfo getSwapchain() const { return m_swapchainInfo; }
        VmaAllocator getVmaAllocator() { return m_memoryAllocator; }
        VkFormat getDepthFormat() const { return m_depthFromat; }

        uint32_t getGraphicsQueueFamilyIndex() const { return m_GraphicsQueueFamily; }
        VkQueue getGraphicsQueue() const { return m_GraphicsQueue; }

        RHI::CommandBuffer& requestCommandBuffer();
        VkFence requestFence();

        VkResult waitForFences(std::vector<VkFence> fences);

        static VkResult submit(VkQueue queue, const RHI::CommandBuffer& cmdBuffer, VkFence fence);
    protected:
        void createInstance();
        void createVKDevice();
        void createSwapchain();

        const GpuInfo& getCurrentUseGpu() const { return m_gpus[m_currentGpuIndex]; }

        void queryGpu();
        void createSurface();
        void pickPhysicalDevice();
        void findQueueFamilies();
        void createLogicalDevice();
        void initVma();
    private:
        bool validateInstanceExtensionAndLayer();

        DeviceCreationParameters        m_deviceCreationParameters{};

        bool                            m_instanceCreated{ false };
        VulkanExtensionSet              m_requiredExtension;
        VulkanExtensionSet              m_optionalExtension; // instance extensions && layer && device extensions in current PC
        std::vector<GpuInfo>            m_gpus;
        // todo(xingyuhang) : if you want to use new feratures, please set this, set in m_deviceCreationParameters
        VkPhysicalDeviceFeatures        m_physicalDevicefeatures;

        VkInstance                      m_instance{ VK_NULL_HANDLE };
        VkDevice                        m_logicalDevice{VK_NULL_HANDLE};
        VkSurfaceKHR                    m_surface;
        uint32_t                        m_currentGpuIndex{};
        SwapchainInfo                   m_swapchainInfo{};

        VkQueue                         m_GraphicsQueue{VK_NULL_HANDLE};
        VkQueue                         m_ComputeQueue{VK_NULL_HANDLE};
        VkQueue                         m_TransferQueue{VK_NULL_HANDLE};
        VkQueue                         m_PresentQueue{VK_NULL_HANDLE};

        int                             m_GraphicsQueueFamily = -1;
        int                             m_ComputeQueueFamily = -1;
        int                             m_TransferQueueFamily = -1;
        int                             m_PresentQueueFamily = -1;
        VmaAllocator                    m_memoryAllocator{ VK_NULL_HANDLE };

        RHI::CommandPool*               m_commandPool{};
        RHI::FencePool*                 m_fencePool{};
        VkFormat                        m_depthFromat{ VK_FORMAT_D32_SFLOAT_S8_UINT };
    };
}

#endif 
