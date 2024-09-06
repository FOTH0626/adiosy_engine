#ifndef AD_VK_RENDER_PASS_H
#define AD_VK_RENDER_PASS_H


#include "AdVKCommon.h"


namespace ade {
  class AdVKDevice;

  struct Attachment{
    int32_t ref = -1;
    VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
  };

  struct RenderSubPass{
    Attachment inputAttachment;
    Attachment colorAttachment;
    Attachment depthStencilAttachment;
    Attachment resolveAttachment;
  };

  class AdVKRenderPass{ 
    public:
      explicit AdVKRenderPass(AdVKDevice *device, const std::vector<VkAttachmentDescription> &attachments = {}, const std::vector<RenderSubPass> &subPass = {});
      ~AdVKRenderPass();

      VkRenderPass GetHandle() const {return mHandle;}
    private:
      VkRenderPass mHandle = VK_NULL_HANDLE;
      AdVKDevice *mDevice;

      std::vector<VkAttachmentDescription> mAttachments;
      std::vector<RenderSubPass> mSubPasses;
  };
}




#endif