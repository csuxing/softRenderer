#include "global_context.h"
#include <log_system.h>
#include "window_system.h"
namespace Jerry
{
    void GlobalContext::startSystems()
    {
        m_loggerSystem = std::make_shared<LogSystem>();

        m_windowSystem = std::make_shared<WindowSystem>();
        // it can read from configue
        WindowCreateInfo create_info{};
        m_windowSystem->initalize(create_info);
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