#include <volk.h>
#include "glfw/glfw3.h"

#include "global_context.h"
#include "log_system.h"
#include "window_system.h"
#include "macro.h"
#include "core/instance.h"

namespace Jerry
{
    void GlobalContext::initVulkan()
    {
        LOG_DEBUG("load vulkan library");
        VkResult result = volkInitialize();
        if (result != VK_SUCCESS)
        {
            LOG_FATAL("vulkan init faild£¡");
        }
        // init instance
        std::unordered_map<const char*, bool> temp{ {"VK_EXT_debug_utils", true}, {"VK_KHR_win32_surface", true}};
        std::vector<const char*> temp1;
        m_instance = std::make_shared<RHI::Instance>("", temp, temp1);
        //create surface
        m_surface = m_windowSystem->create_surface(*(m_instance.get()));
        // select physical gpu
        auto& gpu = m_instance->get_suitable_gpu(m_surface);

    }
    void GlobalContext::startSystems()
    {
        m_loggerSystem = std::make_shared<LogSystem>();

        m_windowSystem = std::make_shared<WindowSystem>();
        // it can read from configue && create window
        WindowCreateInfo create_info{};
        m_windowSystem->initalize(create_info);
        // init vulkan
        initVulkan();
    }
    void GlobalContext::shutdownSystem()
    {
        m_windowSystem.reset();
        m_loggerSystem.reset();
    }
    GlobalContext* GlobalContext::getInstance()
    {
        static GlobalContext instance;
        return &instance;
    }
    GlobalContext::~GlobalContext()
    {
        
    }
}