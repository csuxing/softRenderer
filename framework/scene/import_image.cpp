#include "import_image.h"

#include "fileSystem.h"

namespace SG
{
    std::unique_ptr<ImportImage> ImportImage::load(const std::string& name, const std::string& url, ContentType content_type)
    {
        std::unique_ptr<ImportImage> image{ nullptr };

        auto data = Jerry::read_binary_file(url, 0);
        auto extension = Jerry::get_extension(name);
        if ((extension == "png") || (extension == "jpg"))
        {
            // image =  
        }
        else if (extension == "astc")
        {
        }
        else if (extension == "ktx")
        {

        }
        else if (extension == "ktx2")
        {

        }
        return image;
    }
    ImportImage::ImportImage(const std::string& name, std::vector<uint8_t>&& data, std::vector<Mipmap>&& mipmaps):
        m_name(name)
    {

    }
    std::type_index ImportImage::getType()
    {
        return typeid(ImportImage);
    }
}