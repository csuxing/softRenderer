#ifndef _macro_H_H_
#define _macro_H_H_

#include "global_context.h"
#include "log_system.h"

#define LOG_HELPER(LOG_LEVEL, ...)\
    Jerry::GlobalContext::getInstance()->m_loggerSystem->log(LOG_LEVEL, "[" + std::string(__FUNCTION__) + "] : " + __VA_ARGS__);

#define LOG_DEBUG(...) LOG_HELPER(Jerry::LogSystem::LogLevel::kDebug, __VA_ARGS__);

#define LOG_INFO(...) LOG_HELPER(Jerry::LogSystem::LogLevel::kInfo, __VA_ARGS__);

#define LOG_WARN(...) LOG_HELPER(Jerry::LogSystem::LogLevel::kWarn, __VA_ARGS__);

#define LOG_ERROR(...) LOG_HELPER(Jerry::LogSystem::LogLevel::kError, __VA_ARGS__);

#define LOG_FATAL(...) LOG_HELPER(Jerry::LogSystem::LogLevel::kFatal, __VA_ARGS__);

#define PolitSleep(_ms) std::this_thread::sleep_for(std::chrono::milliseconds(_ms));

#define PolitNameOf(name) #name

#ifndef ROOT_PATH_SIZE
#	define ROOT_PATH_SIZE 0
#endif

#define __FILENAME__ (static_cast<const char *>(__FILE__) + ROOT_PATH_SIZE)
#define LOGI(...) spdlog::info(__VA_ARGS__);
#define LOGW(...) spdlog::warn(__VA_ARGS__);
#define LOGE(...) spdlog::error("[{}:{}] {}", __FILENAME__, __LINE__, fmt::format(__VA_ARGS__));
#define LOGD(...) spdlog::debug(__VA_ARGS__);

#ifdef NDEBUG
#define ASSERT(statement)
#else
#define ASSERT(statement) assert(statement)
#endif

#endif