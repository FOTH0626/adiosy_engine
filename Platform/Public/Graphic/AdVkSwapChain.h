
#ifndef AD_VK_SWAP_CHAIN_H
#define AD_VK_SWAP_CHAIN_H

#include "Graphic/AdVKDevice.h"
#include "Graphic/AdVKGraphicContext.h"
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace ade {
  class AdVKGraphicContext;
  class AdVKDevice;

  struct SurfaceInfo {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR presentMode;
  };

  class AdVKSwapChain{
    public:
      AdVKSwapChain(AdVKGraphicContext *context, AdVKDevice *device);
      ~AdVKSwapChain();

      bool ReCreate();

      const std::vector<VkImage> &GetImages() const {return mImages;}
      uint32_t GetWidth() const {return mSurfaceInfo.capabilities.currentExtent.width;}
      uint32_t GetHeight() const {return mSurfaceInfo.capabilities.currentExtent.height;}

    private:
      void SetupSurfaceCapabilities();
      
      VkSwapchainKHR mHandle = VK_NULL_HANDLE;

      AdVKGraphicContext *mContext;
      AdVKDevice *mDevice;

      SurfaceInfo mSurfaceInfo;

      std::vector<VkImage> mImages;


  };
}



#endif 