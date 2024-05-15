#ifndef _log_system_H_H_
#define _log_system_H_H_

#include <spdlog/spdlog.h>

namespace Jerry
{
    class LogSystem final
    {
    public:
        enum class LogLevel : uint8_t
        {
            kDebug,
            kInfo,
            kWarn,
            kError,
            kFatal
        };

        LogSystem();
        ~LogSystem();

        template<typename... Args>
        void log(LogLevel level, Args&&... args)
        {
            assert(m_logger.get() != nullptr);
            switch (level)
            {
            case Jerry::LogSystem::LogLevel::kDebug:
                m_logger->debug(std::forward<Args>(args)...);
                break;
            case Jerry::LogSystem::LogLevel::kInfo:
                m_logger->info(std::forward<Args>(args)...);
                break;
            case Jerry::LogSystem::LogLevel::kWarn:
                m_logger->warn(std::forward<Args>(args)...);
                break;
            case Jerry::LogSystem::LogLevel::kError:
                m_logger->error(std::forward<Args>(args)...);
                break;
            case Jerry::LogSystem::LogLevel::kFatal:
                m_logger->critical(std::forward<Args>(args)...);
                fatalCallback(std::forward<Args>(args)...);
                break;
            default:
                break;
            }
        }

        template<typename... Args>
        void fatalCallback(Args&&... args)
        {
            const std::string msg = fmt::format(std::forward<Args>(args)...);
            throw std::runtime_error(msg);
        }
    private:
        std::shared_ptr<spdlog::logger> m_logger;

    };
}
#endif
