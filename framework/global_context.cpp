#include "global_context.h"

#include <volk.h>
#include <glfw/glfw3.h>

#include "app/vk_device_manager.h"

#include "rendering/render_context.h"
#include "rendering/render_pass.h"

#include "scene/gltf_loader.h"

#include "log_system.h"
#include "window_system.h"
#include "fileSystem.h"
#include "input_system.h"
#include "macro.h"

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
    }
    void GlobalContext::startSystems()
    {
        m_loggerSystem = std::make_shared<LogSystem>();

        m_windowSystem = std::make_shared<WindowSystem>();

        m_inputSystem = std::make_shared<InputSystem>();

#ifdef RENDER_DOC
        initRenderDoc();
#endif

        // it can read from configue && create window
        WindowCreateInfo create_info{};
        m_windowSystem->initalize(create_info);
        m_inputSystem->initialize();
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

        // load scene
        Scene::GltfLoader loader(static_cast<APP::VkDeviceManager*>(deviceManger));
        std::string assetPath = Jerry::get(Jerry::Type::Assets, "gltfModel");
        auto& scene = loader.read_model_from_file(assetPath + "\\cube.gltf", 0);

        // create pass
        m_forwardRenderpass = new ForwardRenderPass(static_cast<APP::VkDeviceManager*>(deviceManger), m_inputSystem->getCamera());
        m_renderContext = new RHI::RenderContext(static_cast<APP::VkDeviceManager*>(deviceManger));
        m_renderContext->setPass(m_forwardRenderpass);
        m_renderContext->setScene(std::move(scene));

        m_renderContext->init();
    }
    void GlobalContext::shutdownSystem()
    {
        m_windowSystem.reset();
        m_loggerSystem.reset();
        m_inputSystem.reset();
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