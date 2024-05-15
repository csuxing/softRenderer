#ifndef _global_context_H_H_
#define _global_context_H_H_
#include <memory>
namespace Jerry
{
    class LogSystem;
    class WindowSystem;

    class GlobalContext
    {
    public:
        void startSystems();
        static GlobalContext* getInstance();

        std::shared_ptr<LogSystem> m_loggerSystem;
        std::shared_ptr<WindowSystem> m_windowSystem;
    private:
        GlobalContext() = default;
        ~GlobalContext() = default;
    };
}

#endif
