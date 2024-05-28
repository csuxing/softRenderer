#include "render_context.h"
#include "core/device.h"
#include "core/error.h"
#include "swapchain.h"
#include <array>
#include "core/helpers.h"
#include "macro.h"

#include "fileSystem.h"

namespace RHI
{
    RenderContext::RenderContext(Device& device, VkSurfaceKHR surface, const GLFWwindow* window) : 
        m_device(device),
        m_surface(surface),
        m_window(window)
    {
        if (surface != VK_NULL_HANDLE)
        {
            VkSurfaceCapabilitiesKHR surfaceProperties;
            VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.getGpu().getHandle(), 
                surface,
                &surfaceProperties));
            if (surfaceProperties.currentExtent.width == 0xFFFFFFFF)
            {

            }
            else
            {
                m_swapChain = new Swapchain(device, m_surface);
            }
        }
    }
    void RenderContext::init()
    {
        uint32_t imageCount = m_swapChain->getImageCount();
        m_perframes.resize(imageCount);
        for (uint32_t i = 0; i < imageCount; ++i)
        {
            auto& per_frame = m_perframes[i];
            VkFenceCreateInfo info{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
            info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            VK_CHECK(vkCreateFence(m_device.get_handle(), &info, nullptr, &per_frame.queue_submit_fence));

            VkCommandPoolCreateInfo cmd_pool_info{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
            cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
            cmd_pool_info.queueFamilyIndex = m_device.getQueueByFlags(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, 0).getFamilyIndex();
            VK_CHECK(vkCreateCommandPool(m_device.get_handle(), &cmd_pool_info, nullptr, &per_frame.primary_command_pool));

            VkCommandBufferAllocateInfo cmd_buf_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
            cmd_buf_info.commandPool = per_frame.primary_command_pool;
            cmd_buf_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmd_buf_info.commandBufferCount = 1;
            VK_CHECK(vkAllocateCommandBuffers(m_device.get_handle(), &cmd_buf_info, &per_frame.primary_command_buffer));

            per_frame.device = m_device.get_handle();
            per_frame.queue_index = m_device.getQueueByFlags(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, 0).getFamilyIndex();
        }
        initRenderpass();
        initPipeline();
        initFrameBuffer();
    }
    void RenderContext::submit(CommandBuffer& commandBuffer)
    {

    }
    void RenderContext::beginFrame()
    {
        VkSemaphore acquire_semaphore;
        if (m_recycledSemaphores.empty())
        {
            VkSemaphoreCreateInfo info = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
            VK_CHECK(vkCreateSemaphore(m_device.get_handle(), &info, nullptr, &acquire_semaphore));
        }
        else
        {
            acquire_semaphore = m_recycledSemaphores.back();
            m_recycledSemaphores.pop_back();
        }
        VkResult res = vkAcquireNextImageKHR(m_device.get_handle(), m_swapChain->getHandle(), UINT64_MAX, acquire_semaphore, VK_NULL_HANDLE, &m_currentIndex);
        if (res != VK_SUCCESS)
        {
            m_recycledSemaphores.push_back(acquire_semaphore);
            return;
        }

        if (m_perframes[m_currentIndex].queue_submit_fence != VK_NULL_HANDLE)
        {
            vkWaitForFences(m_device.get_handle(), 1, &m_perframes[m_currentIndex].queue_submit_fence, true, UINT64_MAX);
            vkResetFences(m_device.get_handle(), 1, &m_perframes[m_currentIndex].queue_submit_fence);
        }

        if (m_perframes[m_currentIndex].primary_command_pool != VK_NULL_HANDLE)
        {
            vkResetCommandPool(m_device.get_handle(), m_perframes[m_currentIndex].primary_command_pool, 0);
        }

        VkSemaphore old_semaphore = m_perframes[m_currentIndex].swapchain_acquire_semaphore;
        if (old_semaphore != VK_NULL_HANDLE)
        {
            m_recycledSemaphores.push_back(old_semaphore);
        }
        m_perframes[m_currentIndex].swapchain_acquire_semaphore = acquire_semaphore;

        return;
    }
    void RenderContext::frame()
    {
        VkFramebuffer& framebuffer = m_frameBuffers[m_currentIndex];

        VkCommandBuffer& cmd = m_perframes[m_currentIndex].primary_command_buffer;
        VkCommandBufferBeginInfo begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(cmd, &begin_info);
        VkClearValue clear_value;
        clear_value.color = { {0.01f, 0.01f, 0.033f, 1.0f} };
        VkRenderPassBeginInfo rp_begin{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        rp_begin.renderPass = m_renderpass;
        rp_begin.framebuffer = framebuffer;
        rp_begin.renderArea.extent.width = m_swapChain->getExtent2D().width;
        rp_begin.renderArea.extent.height = m_swapChain->getExtent2D().height;
        rp_begin.clearValueCount = 1;
        rp_begin.pClearValues = &clear_value;
        // We will add draw commands in the same command buffer.
        vkCmdBeginRenderPass(cmd, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
        VkViewport vp{};
        vp.width = static_cast<float>(m_swapChain->getExtent2D().width);
        vp.height = static_cast<float>(m_swapChain->getExtent2D().height);
        vp.minDepth = 0.0f;
        vp.maxDepth = 1.0f;
        // Set viewport dynamically
        vkCmdSetViewport(cmd, 0, 1, &vp);

        VkRect2D scissor{};
        scissor.extent.width = m_swapChain->getExtent2D().width;
        scissor.extent.height = m_swapChain->getExtent2D().height;
        // Set scissor dynamically
        vkCmdSetScissor(cmd, 0, 1, &scissor);

        vkCmdDraw(cmd, 3, 1, 0, 0);
        vkCmdEndRenderPass(cmd);
        VK_CHECK(vkEndCommandBuffer(cmd));
    }
    void RenderContext::endFrame()
    {
        VkCommandBuffer& cmd = m_perframes[m_currentIndex].primary_command_buffer;
        if (m_perframes[m_currentIndex].swapchain_release_semaphore == VK_NULL_HANDLE)
        {
            VkSemaphoreCreateInfo semaphore_info{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
            VK_CHECK(vkCreateSemaphore(m_device.get_handle(), &semaphore_info, nullptr,
                &m_perframes[m_currentIndex].swapchain_release_semaphore));
        }
        VkPipelineStageFlags wait_stage{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSubmitInfo info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
        info.commandBufferCount = 1;
        info.pCommandBuffers = &cmd;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &m_perframes[m_currentIndex].swapchain_acquire_semaphore;
        info.pWaitDstStageMask = &wait_stage;
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores = &m_perframes[m_currentIndex].swapchain_release_semaphore;
        // Submit command buffer to graphics queue
        VK_CHECK(vkQueueSubmit(m_device.getQueueByFlags(VK_QUEUE_GRAPHICS_BIT, 0).getHandle(), 1, &info, m_perframes[m_currentIndex].queue_submit_fence));

        VkPresentInfoKHR present{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        present.swapchainCount = 1;
        auto handle = m_swapChain->getHandle();
        present.pSwapchains = &handle;
        present.pImageIndices = &m_currentIndex;
        present.waitSemaphoreCount = 1;
        present.pWaitSemaphores = &m_perframes[m_currentIndex].swapchain_release_semaphore;
        // Present swapchain image
        vkQueuePresentKHR(m_device.getQueueByFlags(VK_QUEUE_GRAPHICS_BIT, 0).getHandle(), &present);
    }

    void RenderContext::initRenderpass()
    {
        VkAttachmentDescription attachment = { 0 };
        attachment.format = m_swapChain->getSurfaceFormat().format;
        attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_ref = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
        VkSubpassDescription subpass = { 0 };
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_ref;

        VkSubpassDependency dependency = { 0 };
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        
        VkRenderPassCreateInfo rp_info = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
        rp_info.attachmentCount = 1;
        rp_info.pAttachments = &attachment;
        rp_info.subpassCount = 1;
        rp_info.pSubpasses = &subpass;
        rp_info.dependencyCount = 1;
        rp_info.pDependencies = &dependency;

        VK_CHECK(vkCreateRenderPass(m_device.get_handle(), &rp_info, nullptr, &m_renderpass));
        LOG_DEBUG("renderpass has created!");
    }
    void RenderContext::initPipeline()
    {
        VkPipelineLayoutCreateInfo layout_info{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        VK_CHECK(vkCreatePipelineLayout(m_device.get_handle(), &layout_info, nullptr, &m_pipelineLayout));

        VkPipelineVertexInputStateCreateInfo vertex_input{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        VkPipelineInputAssemblyStateCreateInfo input_assembly{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        VkPipelineRasterizationStateCreateInfo raster{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        raster.cullMode = VK_CULL_MODE_BACK_BIT;
        raster.frontFace = VK_FRONT_FACE_CLOCKWISE;
        raster.lineWidth = 1.0f;

        VkPipelineColorBlendAttachmentState blend_attachment{};
        blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo blend{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        blend.attachmentCount = 1;
        blend.pAttachments = &blend_attachment;

        VkPipelineViewportStateCreateInfo viewport{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        viewport.viewportCount = 1;
        viewport.scissorCount = 1;

        VkPipelineDepthStencilStateCreateInfo depth_stencil{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };

        VkPipelineMultisampleStateCreateInfo multisample{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        std::array<VkDynamicState, 2> dynamics{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamic{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamic.pDynamicStates = dynamics.data();
        dynamic.dynamicStateCount = to_u32(dynamics.size());

        auto pathVert = Jerry::get(Jerry::Type::Shaders, "triangle.vert.spv");
        auto pathFrag = Jerry::get(Jerry::Type::Shaders, "triangle.frag.spv");
        std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages{};

        shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shader_stages[0].module = loadShader(pathVert.c_str(), m_device.get_handle());
        shader_stages[0].pName = "main";

        shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shader_stages[1].module = loadShader(pathFrag.c_str(), m_device.get_handle());
        shader_stages[1].pName = "main";

        VkGraphicsPipelineCreateInfo pipe{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        pipe.stageCount = to_u32(shader_stages.size());
        pipe.pStages = shader_stages.data();
        pipe.pVertexInputState = &vertex_input;
        pipe.pInputAssemblyState = &input_assembly;
        pipe.pRasterizationState = &raster;
        pipe.pColorBlendState = &blend;
        pipe.pMultisampleState = &multisample;
        pipe.pViewportState = &viewport;
        pipe.pDepthStencilState = &depth_stencil;
        pipe.pDynamicState = &dynamic;

        // We need to specify the pipeline layout and the render pass description up front as well.
        pipe.renderPass = m_renderpass;
        pipe.layout = m_pipelineLayout;

        VK_CHECK(vkCreateGraphicsPipelines(m_device.get_handle(), VK_NULL_HANDLE, 1, &pipe, nullptr, &m_pipeline));
        LOG_DEBUG("pipeline has created!");
        // Pipeline is baked, we can delete the shader modules now.
        vkDestroyShaderModule(m_device.get_handle(), shader_stages[0].module, nullptr);
        vkDestroyShaderModule(m_device.get_handle(), shader_stages[1].module, nullptr);
    }
    void RenderContext::initFrameBuffer()
    {
        VkDevice device = m_device.get_handle();

        for (auto& image_view : m_swapChain->getImageView())
        {
            // Build the framebuffer.
            VkFramebufferCreateInfo fb_info{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };

            fb_info.renderPass = m_renderpass;
            fb_info.attachmentCount = 1;
            fb_info.pAttachments = &image_view;
            fb_info.width = m_swapChain->getExtent2D().width;
            fb_info.height = m_swapChain->getExtent2D().height;
            fb_info.layers = 1;

            VkFramebuffer framebuffer;
            VK_CHECK(vkCreateFramebuffer(device, &fb_info, nullptr, &framebuffer));
            m_frameBuffers.push_back(framebuffer);
        }
        LOG_DEBUG("framebuffer has created!");
    }
}