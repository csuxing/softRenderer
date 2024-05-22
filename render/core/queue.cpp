#include "queue.h"
#include "device.h"
namespace RHI
{
    Queue::Queue(Device& device, uint32_t family_index, VkQueueFamilyProperties properties, VkBool32 can_present, uint32_t index) :
        m_device(device),
        m_familyIndex(family_index),
        m_index(index),
        m_canPresent(can_present),
        m_properties(properties)
    {
        // todo(xing) : init queue
        vkGetDeviceQueue(device.get_handle(), family_index, index, &m_handle);
    }
}