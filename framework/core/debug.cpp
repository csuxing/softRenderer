#include "debug.h"
namespace Utils
{
    // todo(xingyuhang) : debug utils will be impl in the feature
    void DebugUtilsExtDebugUtils::set_debug_name(VkDevice device, VkObjectType object_type, uint64_t object_handle, const char* name) const
    {
    }
    void DebugUtilsExtDebugUtils::set_debug_tag(VkDevice device, VkObjectType object_type, uint64_t object_handle, uint64_t tag_name, const void* tag_data, size_t tag_data_size) const
    {
    }
    void DebugUtilsExtDebugUtils::cmd_begin_label(VkCommandBuffer command_buffer, const char* name, glm::vec4 color) const
    {
    }
    void DebugUtilsExtDebugUtils::cmd_end_label(VkCommandBuffer command_buffer) const
    {
    }
    void DebugUtilsExtDebugUtils::cmd_insert_label(VkCommandBuffer command_buffer, const char* name, glm::vec4 color) const
    {
    }
    void DebugMarkerExtDebugUtils::set_debug_name(VkDevice device, VkObjectType object_type, uint64_t object_handle, const char* name) const
    {
    }
    void DebugMarkerExtDebugUtils::set_debug_tag(VkDevice device, VkObjectType object_type, uint64_t object_handle, uint64_t tag_name, const void* tag_data, size_t tag_data_size) const
    {
    }
    void DebugMarkerExtDebugUtils::cmd_begin_label(VkCommandBuffer command_buffer, const char* name, glm::vec4 color) const
    {
    }
    void DebugMarkerExtDebugUtils::cmd_end_label(VkCommandBuffer command_buffer) const
    {
    }
    void DebugMarkerExtDebugUtils::cmd_insert_label(VkCommandBuffer command_buffer, const char* name, glm::vec4 color) const
    {
    }
    ScopedDebugLabel::ScopedDebugLabel(const DebugUtils& debug_utils, VkCommandBuffer command_buffer, const char* name, glm::vec4 color)
    {
    }
    ScopedDebugLabel::ScopedDebugLabel(const CommandBuffer& command_buffer, const char* name, glm::vec4 color)
    {
    }
    ScopedDebugLabel::~ScopedDebugLabel()
    {
    }
}