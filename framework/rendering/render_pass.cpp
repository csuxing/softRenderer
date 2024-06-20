#include "render_pass.h"

#include "fileSystem.h"
#include "macro.h"
#include "app/vk_device_manager.h"
#include "core/error.h"
#include "core/helpers.h"
#include "core/image.h"
#include "core/image_view.h"
#include "scene/submesh.h"

namespace Jerry
{
    ForwardRenderPass::ForwardRenderPass(APP::VkDeviceManager* manager, Camera* camera) :
        m_deviceManager(manager),
        m_camera(camera)
    {
        const auto& swapchain = m_deviceManager->getSwapchain();
        m_width = swapchain.extent.width;
        m_height = swapchain.extent.height;
        setUpDepthAttachment();
        // create pass
        std::vector<VkAttachmentDescription> attachmentDescs(2);
        attachmentDescs[0].flags = 0;
        attachmentDescs[0].format = swapchain.format.format;
        attachmentDescs[0].samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescs[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescs[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDescs[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescs[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescs[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDescs[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        attachmentDescs[1].flags = 0;
        attachmentDescs[1].format = m_deviceManager->getDepthFormat();
        attachmentDescs[1].samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescs[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescs[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDescs[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescs[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescs[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDescs[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorRef{};
        colorRef.attachment = 0;
        colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthRef{};
        depthRef.attachment = 1;
        depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDesc{};
        subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDesc.colorAttachmentCount = 1;
        subpassDesc.pColorAttachments = &colorRef;
        subpassDesc.pDepthStencilAttachment = &depthRef;
        subpassDesc.inputAttachmentCount = 0;
        subpassDesc.pInputAttachments = nullptr;
        subpassDesc.preserveAttachmentCount = 0;
        subpassDesc.pPreserveAttachments = nullptr;
        subpassDesc.pResolveAttachments = nullptr;

        std::vector<VkSubpassDependency> subpassDependences(2);
        subpassDependences[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        subpassDependences[0].dstSubpass = 0;
        subpassDependences[0].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        subpassDependences[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        subpassDependences[0].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        subpassDependences[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        subpassDependences[0].dependencyFlags = 0;

        subpassDependences[1].srcSubpass = VK_SUBPASS_EXTERNAL;
        subpassDependences[1].dstSubpass = 0;
        subpassDependences[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependences[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependences[1].srcAccessMask = 0;
        subpassDependences[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        subpassDependences[1].dependencyFlags = 0;

        VkRenderPassCreateInfo renderpassCi{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
        renderpassCi.pNext = nullptr;
        renderpassCi.flags = 0;
        renderpassCi.attachmentCount = to_u32(attachmentDescs.size());
        renderpassCi.pAttachments = attachmentDescs.data();
        renderpassCi.subpassCount = 1;
        renderpassCi.pSubpasses = &subpassDesc;
        renderpassCi.dependencyCount = to_u32(subpassDependences.size());
        renderpassCi.pDependencies = subpassDependences.data();

        VkResult res = vkCreateRenderPass(m_deviceManager->getDevice(), &renderpassCi, nullptr, &m_forwardRenderpass);
        if ( res != VK_SUCCESS)
        {
            LOG_ERROR("create renderpass faild!");
        }

        // create framebuffer
        uint32_t frameBufferSize = to_u32(swapchain.images.size());
        m_frameBuffers.resize(frameBufferSize);
        VkFramebufferCreateInfo frameBufferCreateInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
        frameBufferCreateInfo.pNext = nullptr;
        frameBufferCreateInfo.flags = 0;
        frameBufferCreateInfo.renderPass = m_forwardRenderpass;
        std::vector<VkImageView> attachments(2);
        frameBufferCreateInfo.attachmentCount = to_u32(attachments.size());
        frameBufferCreateInfo.pAttachments = attachments.data();
        frameBufferCreateInfo.width = swapchain.extent.width;
        frameBufferCreateInfo.height = swapchain.extent.height;
        frameBufferCreateInfo.layers = 1;
        
        attachments[1] = m_depthView->getHandle();
        for (uint32_t i = 0; i < frameBufferSize; ++i)
        {
            attachments[0] = swapchain.imageViews[i];
            res = vkCreateFramebuffer(m_deviceManager->getDevice(), &frameBufferCreateInfo, nullptr, &m_frameBuffers[i]);
            if (res != VK_SUCCESS)
            {
                LOG_ERROR("create framebuffer failed!");
            }
        }
        // buffer
        m_uboSceneParamsGpu = new RHI::Buffer(m_deviceManager, sizeof(m_uboSceneParams), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
        m_uboSceneParams.projection = m_camera->matrices.perspective;
        m_uboSceneParams.view = m_camera->matrices.view;
        m_uboSceneParamsGpu->update(&m_uboSceneParams, sizeof(m_uboSceneParams));
        m_uboSceneDescriptorBufferInfo.buffer = m_uboSceneParamsGpu->getHandle();
        m_uboSceneDescriptorBufferInfo.offset = 0;
        m_uboSceneDescriptorBufferInfo.range = sizeof(m_uboSceneParams);
        setUpDescriptorPool();
        setUpDescriptorSet();
        setUpPipeline();
    }

    void ForwardRenderPass::draw(SG::Scene* scene, VkCommandBuffer& commandBuffer, uint32_t index)
    {
        updateCamera();
        VkClearValue clear_value[2];
        clear_value[0].color = {{0.5f, 0.5f, 0.5f, 1.0f}};
        clear_value[1].depthStencil = {1.0f, 0xf};
        VkRenderPassBeginInfo rp_begin{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        rp_begin.renderPass = m_forwardRenderpass;
        rp_begin.framebuffer = m_frameBuffers[index];
        rp_begin.renderArea.extent.width = m_width;
        rp_begin.renderArea.extent.height = m_height;
        rp_begin.clearValueCount = 2;
        rp_begin.pClearValues = clear_value;
        // We will add draw commands in the same command buffer.
        vkCmdBeginRenderPass(commandBuffer, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSet, 0, nullptr);
        VkViewport vp{};
        vp.width = static_cast<float>(rp_begin.renderArea.extent.width);
        vp.height = static_cast<float>(rp_begin.renderArea.extent.height);
        vp.minDepth = 0.0f;
        vp.maxDepth = 1.0f;
        // Set viewport dynamically
        vkCmdSetViewport(commandBuffer, 0, 1, &vp);

        VkRect2D scissor{};
        scissor.extent.width = rp_begin.renderArea.extent.width;
        scissor.extent.height = rp_begin.renderArea.extent.height;
        // Set scissor dynamically
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        scene->draw(commandBuffer);

        vkCmdEndRenderPass(commandBuffer);
    }

    void ForwardRenderPass::updateCamera()
    {
        m_uboSceneParams.projection = m_camera->matrices.perspective;
        m_uboSceneParams.view = m_camera->matrices.view;
        m_uboSceneParamsGpu->update(&m_uboSceneParams, sizeof(m_uboSceneParams));
    }

    void ForwardRenderPass::setUpDepthAttachment()
    {
        m_depthFormat = m_deviceManager->getDepthFormat();
        m_depth = new RHI::Image(m_deviceManager, { m_width, m_height, 1 }, m_depthFormat,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
        m_depthView = new RHI::ImageView(m_deviceManager, m_depth, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_UNDEFINED);
    }

    void ForwardRenderPass::setUpDescriptorPool()
    {
        std::vector<VkDescriptorPoolSize> poolSizes = {
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}
        };
        VkDescriptorPoolCreateInfo descriptorPoolInfo{
            VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            nullptr,
            0,
            1,
            to_u32(poolSizes.size()),
            poolSizes.data()
        };
        auto res = vkCreateDescriptorPool(m_deviceManager->getDevice(), &descriptorPoolInfo, nullptr, &m_descriptorPool);
        if ( res!=VK_SUCCESS )
        {
            LOG_ERROR("create descriptor pool failed!");
        }
    }

    void ForwardRenderPass::setUpDescriptorSet()
    {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
            {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_ALL, nullptr}
        };
        VkDescriptorSetLayoutCreateInfo descriptorSetCi{
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            nullptr,
            0,
            to_u32(setLayoutBindings.size()),
            setLayoutBindings.data()
        };
        auto res = vkCreateDescriptorSetLayout(m_deviceManager->getDevice(), &descriptorSetCi, nullptr, &m_descriptorSetLayout);
        if (res != VK_SUCCESS)
        {
            LOG_ERROR("create descriptorSetLayout failed!");
        }
        VkPipelineLayoutCreateInfo pipelineLayoutCi{
            VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            nullptr,
            0,
            1,
            &m_descriptorSetLayout,
            0,
            nullptr
        };
        vkCreatePipelineLayout(m_deviceManager->getDevice(), &pipelineLayoutCi, nullptr, &m_pipelineLayout);

        // allocate descriptor set
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo
        {
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            nullptr,
            m_descriptorPool,
            1,
            &m_descriptorSetLayout
        };
        res = vkAllocateDescriptorSets(m_deviceManager->getDevice(), &descriptorSetAllocateInfo, &m_descriptorSet);
        if (res != VK_SUCCESS)
        {
            LOG_ERROR("allocate descriptorSet failed!");
        }
        // update descriptor set
        VkWriteDescriptorSet writeSet
        {
            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            nullptr,
            m_descriptorSet,
            0,
            0,
            1,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            nullptr,
            &m_uboSceneDescriptorBufferInfo,
            nullptr
        };
        vkUpdateDescriptorSets(m_deviceManager->getDevice(), 1, &writeSet, 0, nullptr);
    }

    void ForwardRenderPass::setUpPipeline()
    {
        VkGraphicsPipelineCreateInfo graphicsPipelineCi;
        graphicsPipelineCi.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphicsPipelineCi.pNext = nullptr;
        graphicsPipelineCi.flags = 0;

        // create shader
        auto pathVert = Jerry::get(Jerry::Type::Shaders, "forwardpass.vert.spv");
        auto pathFrag = Jerry::get(Jerry::Type::Shaders, "forwardpass.frag.spv");
        std::vector<VkPipelineShaderStageCreateInfo> shader_stages(2);
        shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shader_stages[0].module = loadShader(pathVert.c_str(), m_deviceManager->getDevice());
        shader_stages[0].pName = "main";
        shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shader_stages[1].module = loadShader(pathFrag.c_str(), m_deviceManager->getDevice());
        shader_stages[1].pName = "main";

        graphicsPipelineCi.stageCount = to_u32(shader_stages.size());
        graphicsPipelineCi.pStages = shader_stages.data();
        // vertex input state
        const auto& vertexBindingDesc = SG::Vertex::getVertexInputBindDesc();
        const auto& vertexAttributeDesc = SG::Vertex::getVertexInputAttributeDesc();
        VkPipelineVertexInputStateCreateInfo vertexInputStateCi
        {
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            nullptr,
            0,
            1,
            &vertexBindingDesc,
            to_u32(vertexAttributeDesc.size()),
            vertexAttributeDesc.data()
        };
        graphicsPipelineCi.pVertexInputState = &vertexInputStateCi;
        // input assembly state
        VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info
        {
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            nullptr,
            0,
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            VK_FALSE
        };
        graphicsPipelineCi.pInputAssemblyState = &input_assembly_state_create_info;
        // tesselletion state
        VkPipelineTessellationStateCreateInfo tessellation_state_create_info
        {
            VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
            nullptr,
            0,
            0
        };
        graphicsPipelineCi.pTessellationState = nullptr;
        // viewport state
        VkViewport viewport{ 0,0, static_cast<float>(m_width),static_cast<float>(m_height),0,1 };
        VkRect2D scissor{{0,0},{m_width, m_height}};
        VkPipelineViewportStateCreateInfo pipeline_viewport_state_create_info
        {
            VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            nullptr,
            0,
            1,
            &viewport,
            1,
            &scissor
        };
        graphicsPipelineCi.pViewportState = &pipeline_viewport_state_create_info;
        // rasterization
        VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info
        {
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            nullptr,
            0,
            VK_FALSE,
            VK_FALSE,
            VK_POLYGON_MODE_FILL,
            VK_CULL_MODE_NONE,
            VK_FRONT_FACE_COUNTER_CLOCKWISE,
            VK_FALSE,
            0,
            0,
            0,
            1
        };
        graphicsPipelineCi.pRasterizationState = &pipeline_rasterization_state_create_info;
        // multi sampler state
        VkPipelineMultisampleStateCreateInfo multisampleState = {};
        multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleState.pSampleMask = nullptr;

        graphicsPipelineCi.pMultisampleState = &multisampleState;
        // depth stencil state
        VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
        depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilState.depthTestEnable = VK_TRUE;
        depthStencilState.depthWriteEnable = VK_TRUE;
        depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencilState.depthBoundsTestEnable = VK_FALSE;
        depthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
        depthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
        depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
        depthStencilState.stencilTestEnable = VK_FALSE;
        depthStencilState.front = depthStencilState.back;

        graphicsPipelineCi.pDepthStencilState = &depthStencilState;
        // color blend
        VkPipelineColorBlendAttachmentState blendAttachmentState[1] = {};
        blendAttachmentState[0].colorWriteMask = 0xf;
        blendAttachmentState[0].blendEnable = VK_FALSE;
        VkPipelineColorBlendStateCreateInfo colorBlendState = {};
        colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendState.attachmentCount = 1;
        colorBlendState.pAttachments = blendAttachmentState;

        graphicsPipelineCi.pColorBlendState = &colorBlendState;
        // dynamic state
        std::vector<VkDynamicState> states
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamic_state_create_info
        {
            VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            nullptr,
            0,
            to_u32(states.size()),
            states.data()
        };
        graphicsPipelineCi.pDynamicState = &dynamic_state_create_info;
        // pipeline
        graphicsPipelineCi.layout = m_pipelineLayout;
        graphicsPipelineCi.renderPass = m_forwardRenderpass;
        graphicsPipelineCi.subpass = 0;
        graphicsPipelineCi.basePipelineHandle = VK_NULL_HANDLE;
        graphicsPipelineCi.basePipelineIndex = 0;

        VK_CHECK(vkCreateGraphicsPipelines(m_deviceManager->getDevice(), m_deviceManager->getPipelineCache(), 1, &graphicsPipelineCi, nullptr, &m_pipeline));
    }

}
