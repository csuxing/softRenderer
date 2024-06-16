#ifndef _SCENE_H_H_
#define _SCENE_H_H_
#include <memory>
#include <vector>

#include "core/command_buffer.h"

namespace Scene
{
    class SubMesh;
    class Scene
    {
    public:
        void addSubmesh(std::unique_ptr<SubMesh> submesh);
        void draw(VkCommandBuffer& cmd);
    private:
        std::vector<std::unique_ptr<SubMesh>> m_meshs;
    };
}
#endif
