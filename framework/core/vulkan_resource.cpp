#include "vulkan_resource.h"
namespace RHI
{
    namespace Details
    {
        void setDebugName(const Device* device, VkObjectType type, uint64_t handle, const char* debugName)
        {
            if (!debugName || *debugName == '\0' || !device)
            {
                // Can't set name, or no point in setting an empty name
                return;
            }
            // device->get_debug_utils().set_debug_name(device->get_handle(), object_type, handle, debug_name);
        }
    }
}
