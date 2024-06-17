#ifndef _INPUT_SYSTEM_H_H_
#define _INPUT_SYSTEM_H_H_
#include <cstdint>

namespace Jerry
{
    class Camera;

    class InputSystem
    {
    public:
        void onKey(int key, int scanCode, int action, int mods);
        void onCursorPos(double currentCursorX, double currentCursorY);
        void onScroll(double scrollX, double scrollY);

        void initialize();
        void tick();
        void clear();
        Camera* getCamera() const { return m_camera; }
    private:
        Camera* m_camera{};
        float   m_Mousex{ 0.0f };
        float   m_Mousey{ 0.0f };
    };
}

#endif
