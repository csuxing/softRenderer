#include <iostream>
#include <cstring>
#include <random>
#include <direct.h>
#include <spdlog/spdlog.h>

#include "math/geometry.h"
#include "utils/hashHelper.h"
#include "platform/fileSystem.h"
#include "utils/memalign.h"
#include "render/global_context.h"
#include "render/window_system.h"

int main()
{
    // init global context
    auto globalContext = Jerry::GlobalContext::getInstance();
    globalContext->startSystems();
    while (true)
    {
        globalContext->m_windowSystem->pollEvents();
    }
    return 0;
}