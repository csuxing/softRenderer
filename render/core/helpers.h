#ifndef _HELPERS_H_H_
#define _HELPERS_H_H_
#include <stdint.h>

template <class T>
uint32_t to_u32(T value)
{
    static_assert(std::is_arithmetic<T>::value, "T must be numeric");

    if (static_cast<uintmax_t>(value) > static_cast<uintmax_t>(std::numeric_limits<uint32_t>::max()))
    {
        throw std::runtime_error("to_u32() failed, value is too big to be converted to uint32_t");
    }

    return static_cast<uint32_t>(value);
}

#endif
