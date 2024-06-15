#include "render_pass.h"

#include "macro.h"
#include "app/vk_device_manager.h"
#include "core/helpers.h"

namespace Jerry
{
    ForwardRenderPass::ForwardRenderPass(APP::VkDeviceManager* manager, Camera* camera) :
        m_deviceManager(manager),
        m_camera(camera)
    {

        // create pass
        const auto& swapchain = m_deviceManager->getSwapchain();
        std::vector<VkAttachmentDescription> attachmentDescs(2);
        attachmentDescs[0].flags = 0;
        attachmentDescs[0].format = swapchain.format.format;
        attachmentDescs[0].samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescs[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescs[0].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescs[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescs[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescs[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDescs[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        attachmentDescs[1].flags = 0;
        attachmentDescs[1].format = m_deviceManager->getDepthFormat();
        attachmentDescs[1].samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescs[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescs[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
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

        subpassDependences[1].srcSubpass = 0;
        subpassDependences[1].dstSubpass = VK_SUBPASS_EXTERNAL;
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
        m_frameBuffers.resize(swapchain.images.size());
        VkFramebufferCreateInfo frameBufferCreateInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
        frameBufferCreateInfo.pNext = nullptr;
        frameBufferCreateInfo.flags = 0;
        frameBufferCreateInfo.renderPass = m_forwardRenderpass;
        frameBufferCreateInfo.width = swapchain.extent.width;
        frameBufferCreateInfo.height = swapchain.extent.height;
        frameBufferCreateInfo.layers = 1;
        
        
        vkCreateFramebuffer(m_deviceManager->getDevice(), &frameBufferCreateInfo, nullptr, &m_frameBuffers[0]);
    }

    void ForwardRenderPass::setUpDepthAttachment()
    {
        // vk
    }
}
