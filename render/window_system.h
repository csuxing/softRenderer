#ifndef _window_system_H_H_
#define _window_system_H_H_
class GLFWwindow;
namespace Jerry
{
    struct WindowCreateInfo
    {
        int width{ 1280 };
        int height{ 720 };
        const char* title{ "Jerry" };
        bool is_fullscreen{ false };
    };

    class WindowSystem
    {
    public:
        WindowSystem() = default;
        ~WindowSystem() = default;

        void initalize(WindowCreateInfo create_info);
    private:
        GLFWwindow* m_window{ nullptr };
        int m_width{ 0 };
        int m_height{ 0 };
        bool m_isFocusMode{ false };
    };
}

#endif
