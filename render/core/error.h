#ifndef _ERROR_H_H_
#define _ERROR_H_H_

#define VK_CHECK(x)         \
    {                       \
        VkResult err = x;   \
        if (err)            \
        {                   \
            abort();        \
        }                   \
    }
    

#endif
