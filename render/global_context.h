#ifndef _global_context_H_H_
#define _global_context_H_H_
#include <memory>
#include <volk.h>

namespace RHI
{
    class Instance;
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

    protected:
        // load vulkan library && function pointer
        void initVulkan();
    private:
        std::shared_ptr<RHI::Instance> m_instance;
        VkSurfaceKHR m_surface{ VK_NULL_HANDLE };

        GlobalContext() = default;
        ~GlobalContext();
    };
}

#endif
