#ifndef _RENDER_PASS_H_H_
#define _RENDER_PASS_H_H_
#include "render_context.h"
#include <vector>
#include <volk.h>

#include "camera.hpp"
#include "core/buffer.h"

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
    class Image;
    class ImageView;
}

namespace Jerry
{
    struct UBOSceneParams
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
    };
    class ForwardRenderPass
    {
    public:
        explicit ForwardRenderPass(APP::VkDeviceManager* manager, Camera* camera);
        void draw(Scene::Scene* scene, VkCommandBuffer& commandBuffer, uint32_t index);
    protected:
        void setUpDepthAttachment();
        void setUpDescriptorPool();
        void setUpDescriptorSet();
        void setUpPipeline();
    private:
        APP::VkDeviceManager*                m_deviceManager{};
        uint32_t                             m_width{};
        uint32_t                             m_height{};
        VkRenderPass                         m_forwardRenderpass{};
        std::vector<VkFramebuffer>           m_frameBuffers{};
        VkPipeline                           m_pipeline{};

        VkFormat                             m_depthFormat{};
        RHI::Image*                          m_depth{ nullptr };
        RHI::ImageView*                      m_depthView{ nullptr };

        VkPipelineLayout                     m_pipelineLayout{};
        Camera*                              m_camera{ nullptr };

        UBOSceneParams                       m_uboSceneParams;
        RHI::Buffer*                         m_uboSceneParamsGpu{};
        VkDescriptorBufferInfo               m_uboSceneDescriptorBufferInfo;

        VkDescriptorPool                     m_descriptorPool{};
        VkDescriptorSetLayout                m_descriptorSetLayout{};
        VkDescriptorSet                      m_descriptorSet{};
    };
}
#endif
