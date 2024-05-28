#include "device_manager.h"
#include "vk_device_manager.h"
namespace APP
{
    // factory design mode : create all kinds of backend
    DeviceManager* DeviceManager::create(GraphicsAPI api)
    {
        switch (api)
        {
        case APP::DeviceManager::kVk:
            return createVK();
            break;
        case APP::DeviceManager::kD3D11:
            return nullptr;
            break;
        case APP::DeviceManager::kD3D12:
            return nullptr;
            break;
        default:
            return nullptr;
            break;
        }
    }
    DeviceManager* DeviceManager::createVK()
    {
        return new VkDeviceManager();
    }
}