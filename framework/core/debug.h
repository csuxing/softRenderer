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

        virtual void cmd_end_label(VkCommandBuffer command_buffer) const = 0;

        virtual void cmd_insert_label(VkCommandBuffer command_buffer,
            const char* name, glm::vec4 color = {}) const = 0;
    };

    class DebugUtilsExtDebugUtils final : public DebugUtils
    {
    public:
        ~DebugUtilsExtDebugUtils() override = default;

        void set_debug_name(VkDevice device, VkObjectType object_type, uint64_t object_handle,
            const char* name) const override;

        void set_debug_tag(VkDevice device, VkObjectType object_type, uint64_t object_handle,
            uint64_t tag_name, const void* tag_data, size_t tag_data_size) const override;

        void cmd_begin_label(VkCommandBuffer command_buffer,
            const char* name, glm::vec4 color) const override;

        void cmd_end_label(VkCommandBuffer command_buffer) const override;

        void cmd_insert_label(VkCommandBuffer command_buffer,
            const char* name, glm::vec4 color) const override;
    };

    class DebugMarkerExtDebugUtils final : public DebugUtils
    {
    public:
        ~DebugMarkerExtDebugUtils() override = default;

        void set_debug_name(VkDevice device, VkObjectType object_type, uint64_t object_handle,
            const char* name) const override;

        void set_debug_tag(VkDevice device, VkObjectType object_type, uint64_t object_handle,
            uint64_t tag_name, const void* tag_data, size_t tag_data_size) const override;

        void cmd_begin_label(VkCommandBuffer command_buffer,
            const char* name, glm::vec4 color) const override;

        void cmd_end_label(VkCommandBuffer command_buffer) const override;

        void cmd_insert_label(VkCommandBuffer command_buffer,
            const char* name, glm::vec4 color) const override;
    };

    class DummuDebugUtils final : public DebugUtils
    {
    public:
        ~DummuDebugUtils() override = default;

        inline void set_debug_name(VkDevice, VkObjectType, uint64_t, const char*) const override
        {}

        inline void set_debug_tag(VkDevice, VkObjectType, uint64_t,
            uint64_t, const void*, size_t) const override
        {}

        inline void cmd_begin_label(VkCommandBuffer,
            const char*, glm::vec4) const override
        {}

        inline void cmd_end_label(VkCommandBuffer) const override
        {}

        inline void cmd_insert_label(VkCommandBuffer,
            const char*, glm::vec4) const override
        {}
    };

    class CommandBuffer;

    class ScopedDebugLabel final : public DebugUtils
    {
    public:
        ScopedDebugLabel(const DebugUtils& debug_utils, VkCommandBuffer command_buffer,
            const char* name, glm::vec4 color = {});

        ScopedDebugLabel(const CommandBuffer& command_buffer,
            const char* name, glm::vec4 color = {});

        ~ScopedDebugLabel();

    private:
        const DebugUtils* debug_utils;
        VkCommandBuffer   command_buffer;
    };
}
#endif // 
 