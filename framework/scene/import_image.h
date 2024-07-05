#ifndef _IMPORT_IMAGE_H_H_
#define _IMPORT_IMAGE_H_H_
#include <string>
#include <vector>
#include <memory>

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
        enum ContentType
        {
            Unknown,
            Color,
            Other
        };
        static std::unique_ptr<ImportImage> load(const std::string& name, const std::string& url, ContentType content_type);
        ImportImage(const std::string& name, std::vector<uint8_t>&& data, std::vector<Mipmap>&& mipmaps = { {} });
        virtual std::type_index getType() override;
        void coerce_format_to_srgb();
    protected:
        void setData(const uint8_t* rawData, size_t size);
        std::vector<uint8_t>& getMutData();
        void setWidth(uint32_t width);
        void setHeight(uint32_t height);
        void setDepth(uint32_t depth);
        void setLayers(uint32_t layers);
        void setFormat(VkFormat format);
        void set_offsets(const std::vector<std::vector<VkDeviceSize>>& offsets);
        std::vector<Mipmap>& get_mut_mipmaps();
    private:
        std::string m_name;
        std::vector<uint8_t> m_data;
        std::vector<Mipmap> m_mipMaps{ {} };
        uint32_t m_layers{ 1 };
        VkFormat m_format{ VK_FORMAT_UNDEFINED };
        std::vector<std::vector<VkDeviceSize>> m_offsets;
    };
}
#endif
