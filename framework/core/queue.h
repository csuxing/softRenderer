#ifndef _QUEUE_H_H_
#define _QUEUE_H_H_
#include <volk.h>
namespace RHI{
    class Device;
    class Queue
    {
    public:
        Queue(Device& device, uint32_t family_index, VkQueueFamilyProperties properties, VkBool32 can_present, uint32_t index);
        const VkQueueFamilyProperties& getProperties() const
        {
            return m_properties;
        }
        uint32_t getFamilyIndex() const
        {
            return m_familyIndex;
        }
        const VkQueue getHandle() const { return m_handle; }
    protected:

    private:
        Device&                         m_device;
        VkQueue                         m_handle{ VK_NULL_HANDLE };
        uint32_t                        m_familyIndex{ 0 };
        uint32_t                        m_index{ 0 };
        VkBool32                        m_canPresent{ VK_FALSE };
        VkQueueFamilyProperties         m_properties{};
    };
}

#endif