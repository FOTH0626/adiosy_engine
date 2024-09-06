#ifndef AD_VK_DEVICE_H
#define AD_VK_DEVICE_H

#include "Graphic/AdVKGraphicContext.h"
#include "Graphic/AdVKQueue.h"



namespace ade {

  class AdVKGraphicContext;
  class AdVKQueue;

  struct AdVkSettings{
    VkFormat surfaceFormat = VK_FORMAT_B8G8R8A8_UNORM;
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    std::uint32_t swapchainImageCount = 3;
  };

  class AdVKDevice{
    public:
      AdVKDevice(AdVKGraphicContext *context,std::uint32_t graphicQueueCount,std::uint32_t presentQueueCount, const AdVkSettings &settings = {});
      ~AdVKDevice();

      VkDevice GetHandle() const {return mHandle;}

      const AdVkSettings &GetSettings() const {return mSettings;}
    private:
      VkDevice mHandle = VK_NULL_HANDLE;

      std::vector<std::shared_ptr<AdVKQueue>> mGraphicQueues;
      std::vector<std::shared_ptr<AdVKQueue>> mPresentQueues;
      
      AdVkSettings mSettings;
  };
}

#endif
