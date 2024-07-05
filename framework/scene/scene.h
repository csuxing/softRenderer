#ifndef _SCENE_H_H_
#define _SCENE_H_H_
#include <memory>
#include <vector>
#include <string>
#include <typeindex>
#include <unordered_map>

#include "core/command_buffer.h"
#include "basic_define.h"

#include "scene/component.h"
namespace SG
{
    class SubMesh;
    class Scene
    {
    public:
        void addSubmesh(std::unique_ptr<SubMesh> submesh);
        void draw(VkCommandBuffer& cmd);
        void setName(const std::string& name);

        void setComponents(const std::type_index& typeInfo, std::vector<std::unique_ptr<Component>>&& components);

        template<class T>
        void setComponents(std::vector<std::unique_ptr<T>> &&components)
        { 
            std::vector<std::unique_ptr<Component>> result(components.size());
            std::transform(components.begin(), components.end(), result.begin(),
                [](std::unique_ptr<T>& component) -> std::unique_ptr<Component>
                {
                    return std::unique_ptr<Component>(std::move(component));
                });
            setComponents(typeid(T), std::move(result));
        }

    private:
        std::unordered_map<std::type_index, std::vector<std::unique_ptr<Component>>> m_components;
        std::vector<std::unique_ptr<SubMesh>> m_meshs;
        std::string m_name;
    };
}
#endif
