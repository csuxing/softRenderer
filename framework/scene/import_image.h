#ifndef _IMPORT_IMAGE_H_H_
#define _IMPORT_IMAGE_H_H_
#include <string>
#include <vector>

#include <volk.h>

#include "basic_define.h"

#include "component.h"

namespace SG
{
    struct Mipmap
    {
        uint32_t level{0};
        uint32_t offset{0};
        VkExtent3D extent{ 0,0,0 };
    };
    class ImportImage : public Component
    {
    public:
        ImportImage(const std::string& name, std::vector<uint8_t>&& data, std::vector<Mipmap>&& mipmaps = { {} });
        virtual std::type_index getType() override;
    };
}

#endif
