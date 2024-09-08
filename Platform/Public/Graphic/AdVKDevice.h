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
      VkPipelineCache GetPipelineCache() const {return mPipelineCache;}
      AdVKQueue* GetGraphicQueue(uint32_t index) const { return mGraphicQueues.size() < index + 1 ? nullptr : mGraphicQueues[index].get(); };
      AdVKQueue* GetFirstGraphicQueue() const { return mGraphicQueues.empty() ? nullptr : mGraphicQueues[0].get(); };
      AdVKQueue* GetPresentQueue(uint32_t index) const { return mPresentQueues.size() < index + 1 ? nullptr : mPresentQueues[index].get(); };
      AdVKQueue* GetFirstPresentQueue() const { return mPresentQueues.empty() ? nullptr : mPresentQueues[0].get(); };

    private:
      void CreatePipelineCache();

      VkDevice mHandle = VK_NULL_HANDLE;

      std::vector<std::shared_ptr<AdVKQueue>> mGraphicQueues;
      std::vector<std::shared_ptr<AdVKQueue>> mPresentQueues;
      
      AdVkSettings mSettings;

      VkPipelineCache mPipelineCache = VK_NULL_HANDLE;
  };
}

#endif
