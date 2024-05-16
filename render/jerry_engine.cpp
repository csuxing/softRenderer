#include "jerry_engine.h"
#include "global_context.h"
#include "window_system.h"
#include "macro.h"
namespace Jerry
{
    void JerryEngine::startEngine(const std::string& config_file_path)
    {
        auto globalContext = GlobalContext::getInstance();
        globalContext->startSystems();
        LOG_DEBUG("jerry engine start");
    }
    void JerryEngine::shutdownEngine()
    {

    }
    void JerryEngine::initialize()
    {

    }
    void JerryEngine::run()
    {
        std::shared_ptr<WindowSystem> windowSystem = GlobalContext::getInstance()->m_windowSystem;
        ASSERT(windowSystem && "window system must not be nullptr");

        while (!windowSystem->shouldClose())
        {
            const float delta= calculateDeltaTime();
            tickOneFrame(delta);
        }
    }
    bool JerryEngine::tickOneFrame(float delta_time)
    {
        std::shared_ptr<WindowSystem> windowSystem = GlobalContext::getInstance()->m_windowSystem;
        windowSystem->pollEvents();
        return false;
    }
    float JerryEngine::calculateDeltaTime()
    {
        return 0.0f;
    }
}