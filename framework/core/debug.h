#ifndef _DEBUG_H_H_
#define _DEBUG_H_H_

#include <volk.h>
#include <glm/glm.hpp>

namespace Utils
{
    class DebugUtils
    {
    public:
        virtual ~DebugUtils() = default;

        virtual void set_debug_name(VkDevice device, VkObjectType object_type, uint64_t object_handle,
            const char* name) const = 0;

        virtual void set_debug_tag(VkDevice device, VkObjectType object_type, uint64_t object_handle,
            uint64_t tag_name, const void* tag_data, size_t tag_data_size) const = 0;

        virtual void cmd_begin_label(VkCommandBuffer command_buffer,
            const char* name, glm::vec4 color = {}) const = 0;
    };
}

#endif
