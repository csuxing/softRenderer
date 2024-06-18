#include "input_system.h"

#include <memory>
#include <functional>

#include "global_context.h"
#include "window_system.h"
#include "camera.hpp"

namespace Jerry
{
    void InputSystem::onKey(int key, int scanCode, int action, int mods)
    {
        // handle key event
    }

    void InputSystem::onCursorPos(double currentCursorX, double currentCursorY)
    {
        // get current cursorPos
        std::shared_ptr<WindowSystem> windowSystem = GlobalContext::getInstance()->m_windowSystem;
        if (m_Mousex >= 0.0f && m_Mousey >= 0.0f)
        {
            if (windowSystem->isMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
            {
                
            }
            else if (windowSystem->isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
            {
                auto dx = currentCursorX - m_Mousex;
                auto dy = currentCursorY - m_Mousey;
                m_camera->rotate(glm::vec3(0.1 * dy, 0.0, 0.1 * dx));
            }
            else
            {
                
            }
        }
        m_Mousex = static_cast<float>(currentCursorX);
        m_Mousey = static_cast<float>(currentCursorY);
    }

    void InputSystem::onScroll(double scrollX, double scrollY)
    {
        m_camera->translate(glm::vec3(0.0f, 0.0f, scrollY));
    }

    void InputSystem::initialize()
    {
        std::shared_ptr<WindowSystem> windowSystem = GlobalContext::getInstance()->m_windowSystem;
        const auto& extent = windowSystem->getWindowExtent();
        m_camera = new Camera;
        m_camera->type = Camera::CameraType::lookat;
        m_camera->setPosition(glm::vec3(0.0f, 0.0f, -50.0f));
        m_camera->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
        m_camera->setPerspective(60.0f, (float)extent.width / (float)extent.height, 0.1f, 256.0f);

        windowSystem->registerOnKeyFunc(std::bind(&InputSystem::onKey,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::placeholders::_4));

        windowSystem->registerOnCursorPosFunc(std::bind(&InputSystem::onCursorPos,
            this,
            std::placeholders::_1,
            std::placeholders::_2));

        windowSystem->registerOnScrollFunc(std::bind(&InputSystem::onScroll,
            this,
            std::placeholders::_1,
            std::placeholders::_2));
    }

    void InputSystem::tick()
    {
        // update camera info

    }

    void InputSystem::clear()
    {

    }
}
