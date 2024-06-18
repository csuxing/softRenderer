#ifndef _GLOBAL_CONTEXT_H_H_
#define _GLOBAL_CONTEXT_H_H_
#include <memory>
#include <unordered_map>

#include <volk.h>

#ifdef RENDER_DOC
#include "renderdoc_app.h"
#endif

namespace RHI
{
    class RenderContext;
}
namespace Jerry
{
    class LogSystem;
    class WindowSystem;
    class InputSystem;
    class ForwardRenderPass;
    class GlobalContext
    {
    public:
        void startSystems();
        void shutdownSystem();
        static GlobalContext* getInstance();

        std::shared_ptr<LogSystem> m_loggerSystem;
        std::shared_ptr<WindowSystem> m_windowSystem;
        std::shared_ptr<InputSystem> m_inputSystem;

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
        RHI::RenderContext*             m_renderContext;
        ForwardRenderPass*              m_forwardRenderpass{};
        VkSurfaceKHR                    m_surface{ VK_NULL_HANDLE };
        /** @brief Set of device extensions to be enabled for this example and whether they are optional (must be set in the derived constructor) */
        std::unordered_map<const char*, bool> m_deviceExtensions;
        /** @brief Set of instance extensions to be enabled for this example and whether they are optional (must be set in the derived constructor) */
        std::unordered_map<const char*, bool> m_instanceExtensions;
#ifdef RENDER_DOC
        void initRenderDoc();
        RENDERDOC_API_1_1_2* rdoc_api{};
#endif
        GlobalContext() = default;
        ~GlobalContext();
    };
}

#endif
