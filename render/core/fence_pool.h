#ifndef _FENCE_POOL_H__
#define _FENCE_POOL_H__

#include <volk.h>
#include <vector>

namespace RHI
{
    class Device;
    class FencePool
    {
    public:
        FencePool(Device& device);
    private:
        Device&                     m_device;
        std::vector<VkFence>        m_fences;
        uint32_t                    m_activeFenceCount{ 0 };
    };
}

#endif // _FENCE_POOL_H__
