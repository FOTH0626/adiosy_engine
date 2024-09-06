#include "Graphic/AdVKRenderPass.h"
#include "Graphic/AdVKDevice.h"


namespace ade {
  AdVKRenderPass::AdVKRenderPass(AdVKDevice *device, const std::vector<VkAttachmentDescription> &attachments, const std::vector<RenderSubPass> &subPasses)
    :mDevice(device), mAttachments(attachments), mSubPasses(subPasses){
      
    //default subpass and attachment
    if (mSubPasses.empty()) {
      if (attachments.empty()) {
        VkAttachmentDescription defaultColorAttachment = {
          .flags = 0,
          .format = device->GetSettings().surfaceFormat,
          .samples = VK_SAMPLE_COUNT_1_BIT,
          .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
          .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
          .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
          .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
          .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
          .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        };
        mAttachments.push_back(defaultColorAttachment);
      }
      RenderSubPass defaultSubPass = {
        .colorAttachment = {0,VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}

      };
      mSubPasses.push_back(defaultSubPass);
    }
    //subpass
    std::vector<VkSubpassDescription> subpassDescriptions (mSubPasses.size());
    for (int i = 0; i < mSubPasses.size(); ++i) {
      RenderSubPass subpass = mSubPasses[i];
      if (subpass.inputAttachment.ref >= 0 && subpass.inputAttachment.layout == VK_IMAGE_LAYOUT_UNDEFINED) {
        subpass.inputAttachment.layout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
      }
      if (subpass.colorAttachment.ref >= 0 && subpass.colorAttachment.layout == VK_IMAGE_LAYOUT_UNDEFINED) {
        subpass.colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      }
      if (subpass.depthStencilAttachment.ref >= 0 && subpass.depthStencilAttachment.layout == VK_IMAGE_LAYOUT_UNDEFINED) {
        subpass.depthStencilAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
      }
      if (subpass.resolveAttachment.ref >= 0 && subpass.resolveAttachment.layout == VK_IMAGE_LAYOUT_UNDEFINED) {
        subpass.resolveAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      }
      VkAttachmentReference inputAttachmentRef[] = {{ static_cast<uint32_t>(subpass.inputAttachment.ref), subpass.inputAttachment.layout }};
      VkAttachmentReference colorAttachmentRef[] = {{ static_cast<uint32_t>(subpass.colorAttachment.ref), subpass.colorAttachment.layout }};
      VkAttachmentReference depthStencilAttachmentRef[] = {{ static_cast<uint32_t>(subpass.depthStencilAttachment.ref), subpass.depthStencilAttachment.layout }};
      VkAttachmentReference resolveAttachmentRef[] = {{ static_cast<uint32_t>(subpass.resolveAttachment.ref), subpass.resolveAttachment.layout }};
      subpassDescriptions.at(i).flags = 0;
      subpassDescriptions.at(i).pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      subpassDescriptions.at(i).inputAttachmentCount = subpass.inputAttachment.ref >= 0 ? ARRAY_SIZE(inputAttachmentRef) : 0;
      subpassDescriptions.at(i).pInputAttachments = subpass.inputAttachment.ref >= 0 ? inputAttachmentRef : nullptr;
      subpassDescriptions.at(i).colorAttachmentCount = subpass.colorAttachment.ref >= 0 ? ARRAY_SIZE(colorAttachmentRef) : 0;
      subpassDescriptions.at(i).pColorAttachments = subpass.colorAttachment.ref >= 0 ? colorAttachmentRef : nullptr;
      subpassDescriptions.at(i).pResolveAttachments = subpass.resolveAttachment.ref >= 0 ? resolveAttachmentRef : nullptr;
      subpassDescriptions.at(i).pDepthStencilAttachment = subpass.depthStencilAttachment.ref >= 0 ? depthStencilAttachmentRef : nullptr;
      subpassDescriptions.at(i).preserveAttachmentCount = 0;
      subpassDescriptions.at(i).pPreserveAttachments = nullptr;

    }

    std::vector<VkSubpassDependency> dependencies(mSubPasses.size()-1);
    if (mSubPasses.size()>1) {
      for (int i =0; i < dependencies.size(); ++i) {
        dependencies[i].srcSubpass      = i;
        dependencies[i].dstSubpass      = i + 1;
        dependencies[i].srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[i].dstStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[i].srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[i].dstAccessMask   = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
        dependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
      }
    }  
    //createinfo
    VkRenderPassCreateInfo renderPassInfo = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .attachmentCount = static_cast<uint32_t>(mAttachments.size()),
      .pAttachments = mAttachments.data(),
      .subpassCount = static_cast<uint32_t>(mSubPasses.size()),
      .pSubpasses = subpassDescriptions.data(),
      .dependencyCount = static_cast<uint32_t>(dependencies.size()),
      .pDependencies = dependencies.data()
    };
    CALL_VK(vkCreateRenderPass(mDevice->GetHandle(), &renderPassInfo, nullptr, &mHandle));
    LOG_T("RenderPass {0} : {1}, attachment count: {2}, subpass count : {3}", __FUNCTION__,(void*)mHandle, mAttachments.size(),mSubPasses.size());
  }

  AdVKRenderPass::~AdVKRenderPass(){
    VK_D(RenderPass, mDevice->GetHandle(), mHandle);
  }
}