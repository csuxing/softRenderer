#include "import_image.h"

namespace SG
{
    ImportImage::ImportImage(const std::string& name, std::vector<uint8_t>&& data, std::vector<Mipmap>&& mipmaps)
    {
    }
    std::type_index ImportImage::getType()
    {
        return typeid(ImportImage);
    }
}