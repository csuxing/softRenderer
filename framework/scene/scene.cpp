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
}
