#ifndef _SUBMESH_H_H_
#define _SUBMESH_H_H_
#include <glm/glm.hpp>

#include <unordered_map>
#include <string>

#include "core/buffer.h"

#include "component.h"
namespace Scene
{
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec4 joint0;
        glm::vec4 weight0;
    };

    class SubMesh : public Component
    {
    public:
        SubMesh(const std::string& name = {});
        std::unordered_map<std::string, RHI::Buffer> m_vertexBuffers;
    private:
        std::string m_meshName{};
    };
}
#endif
