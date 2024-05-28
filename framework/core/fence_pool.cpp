#include "fence_pool.h"
#include "device.h"
namespace RHI
{
    FencePool::FencePool(Device& device) :
        m_device(device)
    {

    }
}