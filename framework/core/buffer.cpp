#include "buffer.h"
#include "error.h"
RHI::Buffer::Buffer(APP::VkDeviceManager* deviceManager, VkDeviceSize size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags flags):
    m_deviceManager(deviceManager),
    m_size(size)
{
    m_persistent = (flags & VMA_ALLOCATION_CREATE_MAPPED_BIT) != 0;
    VkBufferCreateInfo bufferCi{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufferCi.usage = bufferUsage;
    bufferCi.size = size;

    VmaAllocationCreateInfo memoryInfo{};
    memoryInfo.flags = flags;
    memoryInfo.usage = memoryUsage;

    VmaAllocationInfo allocationInfo{};
    auto result = vmaCreateBuffer(m_deviceManager->getVmaAllocator(), &bufferCi,
        &memoryInfo, &m_handle, &m_allocation, &allocationInfo);

    if (result != VK_SUCCESS)
    {
        throw VulkanException{ result, "cannot create buffer" };
    }

    m_memory = allocationInfo.deviceMemory;

    if (m_persistent)
    {
        m_mappedData = static_cast<uint8_t*>(allocationInfo.pMappedData);
    }
}

RHI::Buffer::Buffer(Buffer&& other):
    m_deviceManager(other.m_deviceManager),
    m_allocation(other.m_allocation),
    m_memory(other.m_memory),
    m_size(other.m_size),
    m_mappedData(other.m_mappedData),
    m_persistent(other.m_persistent),
    m_mapped(other.m_mapped),
    m_handle(other.m_handle)
{
}

void RHI::Buffer::update(const uint8_t* data, size_t size, size_t offset)
{
    if (m_persistent)
    {
        std::copy(data, data + size, m_mappedData + offset);
        flush();
    }
    else
    {
        map(); std::copy(data, data + size, m_mappedData + offset);
        flush();
        unmap();
    }
}

void RHI::Buffer::update(void* data, size_t size, size_t offset)
{
    update(reinterpret_cast<const uint8_t*>(data), size, offset);
}

void RHI::Buffer::flush() const
{
    vmaFlushAllocation(m_deviceManager->getVmaAllocator(), m_allocation, 0, m_size);
}

uint8_t* RHI::Buffer::map()
{
    if (!m_mapped && !m_mappedData)
    {
        vmaMapMemory(m_deviceManager->getVmaAllocator(), m_allocation, reinterpret_cast<void**>(&m_mappedData));
        m_mapped = true;
    }
    return m_mappedData;
}

void RHI::Buffer::unmap()
{
    if (m_mapped)
    {
        vmaUnmapMemory(m_deviceManager->getVmaAllocator(), m_allocation);
        m_mapped = false;
        m_mappedData = nullptr;
    }
}
