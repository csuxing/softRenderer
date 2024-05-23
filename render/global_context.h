#ifndef _global_context_H_H_
#define _global_context_H_H_
#include <memory>
#include <unordered_map>
#include <volk.h>

namespace RHI
{
    class Instance;
    class Device;
    class RenderContext;
}
namespace Jerry
{
    class LogSystem;
    class WindowSystem;

    class GlobalContext
    {
    public:
        void startSystems();
        void shutdownSystem();
        static GlobalContext* getInstance();

        std::shared_ptr<LogSystem> m_loggerSystem;
        std::shared_ptr<WindowSystem> m_windowSystem;
        void beginFrame();
        void frame();
        void endFrame();
    protected:
        // load vulkan library && function pointer
        void initVulkan();
        void addDviceExtension(const char* extension)
        {
            m_deviceExtensions.insert({ extension, true });
        }
        const std::unordered_map<const char*, bool>& getDeviceExtensions() const noexcept
        {
            return m_deviceExtensions;
        }
        void addInstanceExtension(const char* extension)
        {
            m_instanceExtensions.insert({ extension, true });
        }
    private:
        std::shared_ptr<RHI::Instance>  m_instance;
        std::shared_ptr<RHI::Device>    m_device;
        RHI::RenderContext*             m_renderContext;
        VkSurfaceKHR                    m_surface{ VK_NULL_HANDLE };
        /** @brief Set of device extensions to be enabled for this example and whether they are optional (must be set in the derived constructor) */
        std::unordered_map<const char*, bool> m_deviceExtensions;
        /** @brief Set of instance extensions to be enabled for this example and whether they are optional (must be set in the derived constructor) */
        std::unordered_map<const char*, bool> m_instanceExtensions;

        GlobalContext() = default;
        ~GlobalContext();
    };
}

#endif
