#ifndef _COMMAND_POOL_H__
#define _COMMAND_POOL_H__
#include <cstdint>
#include "command_buffer.h"
namespace RHI
{
    class RenderFrame;
    class Device;
    class CommandPool
    {
    public:
        CommandPool(Device& device, uint32_t queueFamilyIndex, RenderFrame* render_frame = nullptr,
            size_t                   thread_index = 0,
            CommandBuffer::ResetMode reset_mode = CommandBuffer::ResetMode::ResetPool);
    protected:
    private:
    };
}

#endif // _COMMAND_POOL_H__
