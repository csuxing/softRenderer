#include <volk.h>
#include "window_system.h"
#include "macro.h"
#include "core/instance.h"

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

        glfwSetWindowUserPointer(m_window, this);
        glfwSetKeyCallback(m_window, keyCallback);
        glfwSetCharCallback(m_window, charCallback);
        glfwSetCharModsCallback(m_window, charModsCallback);
        glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
        glfwSetCursorPosCallback(m_window, cursorPosCallback);
        glfwSetCursorEnterCallback(m_window, cursorEnterCallback);
        glfwSetScrollCallback(m_window, scrollCallback);
        glfwSetDropCallback(m_window, dropCallback);
        glfwSetWindowSizeCallback(m_window, windowSizeCallback);
        glfwSetWindowCloseCallback(m_window, windowCloseCallback);

        glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);

        uint32_t glfwExtCount;
        const char** glfwExt = glfwGetRequiredInstanceExtensions(&glfwExtCount);
        ASSERT(glfwExt);

        for (uint32_t i = 0; i < glfwExtCount; ++i)
        {
            m_requiredExtensions.push_back(std::string(glfwExt[i]));
        }
        m_camera = new Camera;
        m_camera->type = Camera::CameraType::lookat;
        m_camera->setPosition(glm::vec3(0.0f, 0.0f, -6.0f));
        m_camera->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
        m_camera->setPerspective(60.0f, (float)m_width / (float)m_height, 0.1f, 256.0f);
        // todo(xingyuhang) : register callback, handle mouse move and push button
    }
    void WindowSystem::pollEvents() const
    {
        glfwPollEvents();
    }
    bool WindowSystem::shouldClose() const
    {
        return glfwWindowShouldClose(m_window);
    }
    VkSurfaceKHR WindowSystem::create_surface(RHI::Instance &instance) noexcept
    {

        if (instance.get_handle() == VK_NULL_HANDLE || !m_window)
        {
            return VK_NULL_HANDLE;
        }

        VkSurfaceKHR surface;

        VkResult errCode = glfwCreateWindowSurface(instance.get_handle(), m_window, NULL, &surface);

        if (errCode != VK_SUCCESS)
        {
            return nullptr;
        }

        return surface;
    }

    void WindowSystem::setFocusMode(bool mode)
    {
        m_isFocusMode = mode;
        glfwSetInputMode(m_window, GLFW_CURSOR, m_isFocusMode ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
}