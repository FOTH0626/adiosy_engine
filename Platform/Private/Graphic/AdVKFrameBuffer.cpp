#include "Graphic/AdVKFrameBuffer.h"
#include "Graphic/AdVKCommon.h"
#include "Graphic/AdVKImageView.h"


namespace ade {
  AdVKFrameBuffer::AdVKFrameBuffer(AdVKDevice *device, AdVKRenderPass *renderPass, const std::vector<VkImage> &images, std::uint32_t width, std::uint32_t height)
  :mDevice(device), mRenderPass(renderPass), mWidth(width), mHeight(height) {
    ReCreate(images, width, height);
  }

  AdVKFrameBuffer::~AdVKFrameBuffer(){
    VK_D(Framebuffer, mDevice->GetHandle(), mHandle);
  }

  bool AdVKFrameBuffer::ReCreate(const std::vector<VkImage> &images, uint32_t width, uint32_t height){
    VkResult ret;

    VK_D(Framebuffer, mDevice->GetHandle(), mHandle);

    mWidth = width;
    mHeight = height;
    mImageViews.clear();

    VkImageView attachments[images.size()];
    for (int i = 0; i < images.size(); ++i) {
      mImageViews.push_back(std::make_shared<AdVKImageView>(mDevice,images[i],mDevice->GetSettings().surfaceFormat,VK_IMAGE_ASPECT_COLOR_BIT));
      attachments[i] = mImageViews[i]->GetHandle();
    }

    VkFramebufferCreateInfo frameBufferInfo = {
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .renderPass = mRenderPass->GetHandle(),
      .attachmentCount = static_cast<uint32_t>(mImageViews.size()),
      .pAttachments = attachments,
      .width = width,
      .height = height,
      .layers = 1
    };  

    ret = vkCreateFramebuffer(mDevice->GetHandle(), &frameBufferInfo, nullptr, &mHandle);
    LOG_T("FrameBuffer {0}, new: {1}, width: {2}, height: {3}, view count: {4}", __FUNCTION__, (void*)mHandle, mWidth, mHeight, mImageViews.size());
    return ret == VK_SUCCESS;
  }
}