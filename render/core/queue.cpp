#include "queue.h"
namespace RHI
{
    Queue::Queue(Device& device, uint32_t family_index, VkQueueFamilyProperties properties, VkBool32 can_present, uint32_t index) :
        m_device(device)
    {

    }
}