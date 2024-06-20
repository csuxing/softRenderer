#ifndef _COMPONENT_H_H_
#define _COMPONENT_H_H_
#include <string>
#include <typeindex>

#include "basic_define.h"

namespace SG
{
    class Component
    {
    public:
        Component() = default;
        Component(Component&& other) = default;
        virtual ~Component() = default;

        Component(const std::string name);
        const std::string& getName() const { return m_name; }
        virtual std::type_index getType() = 0;
    private:
        std::string m_name{};
    };
}
#endif
