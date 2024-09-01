#ifndef AD_VK_DEVICE_H
#define AD_VK_DEVICE_H

#include "Graphic/AdVKGraphicContext.h"
#include "Graphic/AdVKQueue.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>


namespace ade {

  class AdVKGraphicContext;
  class AdVKQueue;

  struct AdVkSettings{

  };

  class AdVKDevice{
    public:
      AdVKDevice(AdVKGraphicContext *context,uint32_t graphicQueueCount,uint32_t presentQueueCount, const AdVkSettings &settings = {});
      ~AdVKDevice();
    private:
      VkDevice mDevice;

      std::vector<std::shared_ptr<AdVKQueue>> mGraphicQueues;
      std::vector<std::shared_ptr<AdVKQueue>> mPresentQueues;
      
  };
}

#endif
