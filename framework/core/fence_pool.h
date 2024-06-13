#ifndef _FENCE_POOL_H__
#define _FENCE_POOL_H__

#include <volk.h>
#include <vector>

namespace APP
{
    class VkDeviceManager;
}
namespace RHI
{
    class FencePool
    {
    public:
        FencePool(APP::VkDeviceManager* deviceManager);
        VkFence requestFence();
    private:
        APP::VkDeviceManager* m_deviceManager;
        std::vector<VkFence>    m_fences;
        uint32_t                m_activeFenceCount;
    };
}

#endif // _FENCE_POOL_H__
