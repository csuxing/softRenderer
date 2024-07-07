#ifndef _VK_COMMON_H_H_
#define _VK_COMMON_H_H_

#include <volk.h>

namespace RHI
{
    struct ImageMemoryBarrier
    {
        VkPipelineStageFlags src_stage_mask{ VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT };

        VkPipelineStageFlags dst_stage_mask{ VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT };

        VkAccessFlags src_access_mask{ 0 };

        VkAccessFlags dst_access_mask{ 0 };

        VkImageLayout old_layout{ VK_IMAGE_LAYOUT_UNDEFINED };

        VkImageLayout new_layout{ VK_IMAGE_LAYOUT_UNDEFINED };

        uint32_t old_queue_family{ VK_QUEUE_FAMILY_IGNORED };

        uint32_t new_queue_family{ VK_QUEUE_FAMILY_IGNORED };
    };
    bool is_depth_only_format(VkFormat format);
    bool is_depth_stencil_format(VkFormat format);
}

#endif