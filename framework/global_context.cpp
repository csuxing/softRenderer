#include <volk.h>
#include "glfw/glfw3.h"
#include "global_context.h"
#include "log_system.h"
#include "window_system.h"
#include "macro.h"
#include "core/instance.h"
#include "core/device.h"
#include "rendering/render_context.h"

#include "app/vk_device_manager.h"

#ifdef RENDER_DOC
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#define RC_INVOKED
#define _X86_
#include <libloaderapi.h>
#endif

namespace Jerry
{
    void GlobalContext::initVulkan()
    {
        LOG_DEBUG("load vulkan library");
        VkResult result = volkInitialize();
        if (result != VK_SUCCESS)
        {
            LOG_FATAL("vulkan init faild£¡");
        }
        // init instance
        std::unordered_map<const char*, bool> temp{ {"VK_EXT_debug_utils", true}, {"VK_KHR_win32_surface", true}};
        std::vector<const char*> temp1;
        m_instance = std::make_shared<RHI::Instance>("", temp, temp1);
        m_instance->init();
        //create surface
        m_surface = m_windowSystem->create_surface(*(m_instance.get()));
        // select physical gpu
        auto& gpu = m_instance->get_suitable_gpu(m_surface);
        if (m_instance->is_enabled(VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME))
        {
            addDviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
            if (m_instanceExtensions.find(VK_KHR_DISPLAY_EXTENSION_NAME) != m_instanceExtensions.end())
            {
                addInstanceExtension(VK_KHR_DISPLAY_SWAPCHAIN_EXTENSION_NAME);
            }
        }
        // create device
        m_device = std::make_shared<RHI::Device>(gpu, m_surface, getDeviceExtensions());
        //m_renderContext = new RHI::RenderContext(*(m_device.get()), m_surface, m_windowSystem->getWindow());
        //m_renderContext->init();
    }
    void GlobalContext::startSystems()
    {
        m_loggerSystem = std::make_shared<LogSystem>();

        m_windowSystem = std::make_shared<WindowSystem>();

#ifdef RENDER_DOC
        initRenderDoc();
#endif

        // it can read from configue && create window
        WindowCreateInfo create_info{};
        m_windowSystem->initalize(create_info);

        // device manager create vulkan resource
        auto deviceManger = APP::DeviceManager::create(APP::DeviceManager::kVk);
        APP::DeviceCreationParameters parameters;
        parameters.m_instanceParameters.enableDebugRuntime = true;
        parameters.m_window = m_windowSystem->getWindow();
        std::vector<std::string> instanceExtensions = m_windowSystem->getRequiredExtension();
        for (auto ext : instanceExtensions)
        {
            parameters.m_instanceParameters.requiredVulkanInstanceExtensions.push_back(ext);
        }
        parameters.requiredVulkanDeviceExtensions.push_back("VK_KHR_swapchain");
        deviceManger->createDevice(parameters);
        m_renderContext = new RHI::RenderContext(static_cast<APP::VkDeviceManager*>(deviceManger));
        m_renderContext->init();
        // init vulkan
        //initVulkan();
    }
    void GlobalContext::shutdownSystem()
    {
        m_windowSystem.reset();
        m_loggerSystem.reset();
    }
    GlobalContext* GlobalContext::getInstance()
    {
        static GlobalContext instance;
        return &instance;
    }
    void GlobalContext::frame()
    {
        m_renderContext->frame();
    }

    void GlobalContext::beginFrame()
    {
#ifdef RENDER_DOC
        if (rdoc_api)
        {
            auto pointer = RENDERDOC_DEVICEPOINTER_FROM_VKINSTANCE(m_instance->get_handle());
            auto window = m_windowSystem->getWindow();
            HWND hwnd = glfwGetWin32Window(window);
            rdoc_api->StartFrameCapture(pointer, hwnd);
        }
#endif
        m_renderContext->beginFrame();
    }

    void GlobalContext::endFrame()
    {
        m_renderContext->endFrame();
#ifdef RENDER_DOC
        if (rdoc_api)
        {
            auto pointer = RENDERDOC_DEVICEPOINTER_FROM_VKINSTANCE(m_instance->get_handle());
            auto window = m_windowSystem->getWindow();
            HWND hwnd = glfwGetWin32Window(window);
            rdoc_api->EndFrameCapture(pointer, hwnd);
        }
#endif
    }
#ifdef RENDER_DOC
    void GlobalContext::initRenderDoc()
    {
        HMODULE mod = GetModuleHandleA("renderdoc.dll");
        
        if (mod)
        {
            pRENDERDOC_GetAPI RENDERDOC_GetAPI =
                (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
            int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_5_0, (VOID**)&rdoc_api);
            // assert(ret == 1);
        }
        LOG_DEBUG("render doc init!");
    }
#endif
    GlobalContext::~GlobalContext()
    {
        
    }
}