#ifndef _RENDER_PASS_H_H_
#define _RENDER_PASS_H_H_
#include "render_context.h"
#include <vector>
#include <volk.h>

#include "camera.hpp"

namespace APP
{
    class VkDeviceManager;
}

namespace Scene
{
    class Scene;
}

namespace RHI
{
    class CommandBuffer;
}
namespace Jerry
{
    class ForwardRenderPass
    {
    public:
        explicit ForwardRenderPass(APP::VkDeviceManager* manager, Camera* camera);
        void draw(Scene::Scene* scene, RHI::CommandBuffer& commandBuffer);
    protected:
        void setUpDepthAttachment();
    private:
        APP::VkDeviceManager* m_deviceManager{};
        VkRenderPass m_forwardRenderpass{};
        std::vector<VkFramebuffer> m_frameBuffers{};
        VkPipeline                  m_pipeline{};
        VkDescriptorSet m_descriptor{};

        VkImage         m_depth{};
        VkImageView     m_depthView{};

        VkPipelineLayout m_pipelineLayout{};
        Camera* m_camera;
    };
}
#endif
