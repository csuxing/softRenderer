#include "window_system.h"

#include "GLFW/glfw3.h"
#include "macro.h"

namespace Jerry
{
    void WindowSystem::initalize(WindowCreateInfo create_info)
    {
        if (!glfwInit())
        {
            LOG_FATAL(__FUNCTION__, "failed to initialize GLFW!");
        }

        m_width = create_info.width;
        m_height = create_info.height;
        
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_window = glfwCreateWindow(m_width, m_height, create_info.title, nullptr, nullptr);
        if (!m_window)
        {
            LOG_FATAL(__FUNCTION__, "failed to create Window");
            glfwTerminate();
            return;
        }

        glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
    }
}