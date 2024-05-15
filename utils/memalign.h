#ifndef _memalign_H_H_
#define _memalign_H_H_
#include <assert.h>
#include <corecrt_malloc.h>
namespace Utils
{
    void* alignd_alloc(size_t size, size_t align) noexcept
    {
        align = (align < sizeof(void*)) ? sizeof(void*) : align;
        assert(align && !(align & align - 1));
        assert((align % sizeof(void*)) == 0);

        void* p = nullptr;

        p = ::_aligned_malloc(size, align);

        return p;
    }
}

#endif
