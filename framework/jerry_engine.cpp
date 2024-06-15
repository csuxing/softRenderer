#include "jerry_engine.h"
#include "global_context.h"
#include "input_system.h"
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
        auto globalContext = GlobalContext::getInstance();
        globalContext->shutdownSystem();
    }
    void JerryEngine::initialize()
    {
        // initilize scene
    }
    void JerryEngine::run()
    {
        std::shared_ptr<WindowSystem> windowSystem = GlobalContext::getInstance()->m_windowSystem;
        ASSERT(windowSystem && "window system must not be nullptr");

        while (!windowSystem->shouldClose())
        {
            const float delta= calculateDeltaTime();
            // LOG_DEBUG(std::to_string(delta));
            tickOneFrame(delta);
        }
    }
    bool JerryEngine::tickOneFrame(float delta_time)
    {
        std::shared_ptr<WindowSystem> windowSystem = GlobalContext::getInstance()->m_windowSystem;
        std::shared_ptr<InputSystem>  inputSystem = GlobalContext::getInstance()->m_inputSystem;
        windowSystem->pollEvents();
        inputSystem->tick();
        // tick logical
        tickLogical();
        // tick render
        tickRender();
        return true;
    }
    void JerryEngine::tickRender()
    {
        auto globalContext = GlobalContext::getInstance();
        globalContext->beginFrame();
        globalContext->frame();
        globalContext->endFrame();
    }
    float JerryEngine::calculateDeltaTime()
    {
        float deltaTime{};
        {
            using namespace std::chrono;
            steady_clock::time_point tickTimePoint = steady_clock::now();
            duration<float> timeSpan = duration_cast<duration<float>>(tickTimePoint - m_lastTickTimePoint);
            deltaTime = timeSpan.count();
            m_lastTickTimePoint = tickTimePoint;
        }
        return deltaTime;
    }
}