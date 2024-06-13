#ifndef _SUBMESH_H_H_
#define _SUBMESH_H_H_
#include <glm/glm.hpp>
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

    class SubMesh
    {
    public:

    protected:

    private:
    };
}
#endif
