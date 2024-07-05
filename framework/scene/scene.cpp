#include "scene.h"
#include "submesh.h"
namespace SG
{
    void Scene::addSubmesh(std::unique_ptr<SubMesh> submesh)
    {
        m_meshs.push_back(std::move(submesh));
    }

    void Scene::draw(VkCommandBuffer& cmd)
    {
        auto length = m_meshs.size();
        for (decltype(length) i = 0; i < length; ++i)
        {
            m_meshs[i]->draw(cmd);
        }
    }
    void Scene::setName(const std::string& name)
    {
        m_name = name;
    }
    void Scene::setComponents(const std::type_index& typeInfo, std::vector<std::unique_ptr<Component>>&& components)
    {
        m_components[typeInfo] = std::move(components);
    }
}
