#ifndef _BUFFER_H_H_
#define _BUFFER_H_H_

#include <vulkan/vulkan_core.h>
#include <vk_mem_alloc.h>
#include "app/vk_device_manager.h"
namespace RHI
{
    class Buffer
    {
    public:
        Buffer(APP::VkDeviceManager*    deviceManager,
            VkDeviceSize                size,
            VkBufferUsageFlags          bufferUsage,
            VmaMemoryUsage              memoryUsage,
            VmaAllocationCreateFlags =  VMA_ALLOCATION_CREATE_MAPPED_BIT);
        void update(const uint8_t* data, size_t size, size_t offset = 0);
        void update(void* data, size_t size, size_t offset = 0);
        void flush() const;
        uint8_t* map();
        void unmap();
        VkBuffer getHandle() const noexcept { return m_handle; }
    protected:
    private:
        APP::VkDeviceManager* m_deviceManager{};
        VmaAllocation         m_allocation{};
        VkDeviceMemory        m_memory{};
        VkDeviceSize          m_size{};
        uint8_t*              m_mappedData{nullptr};
        bool                  m_persistent{false};
        bool                  m_mapped{false};

        VkBuffer              m_handle{};
    };
}
#endif
