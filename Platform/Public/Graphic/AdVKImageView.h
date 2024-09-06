#ifndef AD_VK_IMAGE_VIEW_H
#define AD_VK_IMAGE_VIEW_H

#include "Graphic/AdVKCommon.h"


namespace ade {    
  class AdVKDevice;

  class AdVKImageView{
    public:
      AdVKImageView(AdVKDevice *device, VkImage image, VkFormat format, VkImageAspectFlags VkImageAspectFlags);
      ~AdVKImageView();
      
      VkImageView GetHandle() const { return mHandle; }
    private:
      VkImageView mHandle = VK_NULL_HANDLE;

      AdVKDevice *mDevice;
  };
}

#endif