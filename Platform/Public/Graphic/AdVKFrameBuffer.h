#ifndef AD_VK_FRAME_BUFFER_H
#define AD_VK_FRAME_BUFFER_H

#include "Graphic/AdVKDevice.h"
#include "Graphic/AdVKImageView.h"
#include "Graphic/AdVKRenderPass.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>


namespace ade {
  class AdVKDevice;
  class AdVKRenderPass;
  class AdVKImageView;
  
  class AdVKFrameBuffer{
    public:
      AdVKFrameBuffer(AdVKDevice *device, AdVKRenderPass *renderPass, const std::vector<VkImage> &images, std::uint32_t width, std::uint32_t height);
      ~AdVKFrameBuffer();

      bool ReCreate(const std::vector<VkImage> &images, std::uint32_t width, std::uint32_t height);

      [[nodiscard]] VkFramebuffer GetHandle() const {return mHandle;}
      [[nodiscard]] uint32_t GetWidth() const { return mWidth;}
      [[nodiscard]] uint32_t GetHeight() const { return mHeight;}

    private:
      VkFramebuffer mHandle = VK_NULL_HANDLE;
      AdVKDevice *mDevice;
      AdVKRenderPass *mRenderPass;
      uint32_t mWidth;
      uint32_t mHeight;
      std::vector<std::shared_ptr<AdVKImageView>> mImageViews;
  };
}

#endif