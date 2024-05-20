#ifndef _VULKAN_RESOURCE_H__
#define _VULKAN_RESOURCE_H__
#include <volk.h>
#include <string>
namespace RHI
{
    class Device;
    namespace Details
    {
        void setDebugName(const Device* device, VkObjectType type, uint64_t handle, const char* debugName);
    }
    
    template<typename THandle, VkObjectType OBJECT_TYPE, typename Device = RHI::Device>
    class VulkanResource
    {
    public:
        VulkanResource(THandle handle = VK_NULL_HANDLE, Device* device = nullptr) :
            m_handle{ handle }, m_device{ device }
        {
        }

        VulkanResource(const VulkanResource&) = delete;
        VulkanResource& operator=(const VulkanResource&) = delete;

        VulkanResource(VulkanResource&& other) :
            m_handle{ other.m_handle }, m_device{ other.m_device }
        {
            set_debug_name(other.m_debugName);

            other.m_handle = VK_NULL_HANDLE;
        }

        VulkanResource& operator=(VulkanResource&& other)
        {
            m_handle = other.m_handle;
            m_device = other.m_device;
            set_debug_name(other.m_debugName);

            other.m_handle = VK_NULL_HANDLE;

            return *this;
        }

        virtual ~VulkanResource() = default;

        inline VkObjectType get_object_type() const
        {
            return OBJECT_TYPE;
        }

        inline Device& get_device() const
        {
            assert(m_device && "Device handle not set");
            return *m_device;
        }

        inline const THandle& get_handle() const
        {
            return m_handle;
        }

        inline const uint64_t get_handle_u64() const
        {
            // See https://github.com/KhronosGroup/Vulkan-Docs/issues/368 .
            // Dispatchable and non-dispatchable handle types are *not* necessarily binary-compatible!
            // Non-dispatchable handles _might_ be only 32-bit long. This is because, on 32-bit machines, they might be a typedef to a 32-bit pointer.
            using UintHandle = typename std::conditional<sizeof(THandle) == sizeof(uint32_t), uint32_t, uint64_t>::type;

            return static_cast<uint64_t>(reinterpret_cast<UintHandle>(m_handle));
        }

        inline const std::string& get_debug_name() const
        {
            return m_debugName;
        }

        inline void set_debug_name(const std::string& name)
        {
            m_debugName = name;
            detail::set_debug_name(m_device, OBJECT_TYPE, get_handle_u64(), m_debugName.c_str());
        }
    protected:
        THandle         m_handle;
        Device*         m_device;
        std::string     m_debugName;

    };
}

#endif // _VULKAN_RESOURCE_H__
