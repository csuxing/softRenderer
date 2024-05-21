#ifndef _COMMAND_BUFFER_H__
#define _COMMAND_BUFFER_H__

namespace RHI
{
    class CommandBuffer
    {
    public:
        enum class ResetMode
        {
            ResetPool,
            ResetIndividually,
            AlwaysAllocate,
        };

        enum class State
        {
            Invalid,
            Initial,
            Recording,
            Executable,
        };
    protected:
    private:
    };
}
#endif // _COMMAND_BUFFER_H__
