#ifndef _KTX_H_H_
#define _KTX_H_H_

#include "import_image.h"

namespace SG
{
    class Ktx : public ImportImage
    {
    public:
        Ktx(const std::string& name, const std::vector<uint8_t>& data, ContentType content_type);
        virtual ~Ktx() = default;
    };
}
#endif
