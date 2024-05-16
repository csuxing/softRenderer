#ifndef _jerry_engine_H_H_
#define _jerry_engine_H_H_
#include <string>
#include <chrono>
namespace Jerry
{
    class JerryEngine
    {
    public:
        void startEngine(const std::string& config_file_path);
        void shutdownEngine();

        void initialize();

        void run();
    protected:

        bool tickOneFrame(float delta_time);
        float calculateDeltaTime();

        bool m_isQuit{ false };
        std::chrono::steady_clock::time_point m_lastTickTimePoint{ std::chrono::steady_clock::now() };
        float m_averageDuration{ 0.0f };
        int m_frameCount{ 0 };
        int m_fps{ 0 };
    };
}
#endif
