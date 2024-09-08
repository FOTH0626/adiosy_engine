#ifndef AD_VK_COMMAND_BUFFER_H
#define AD_VK_COMMAND_BUFFER_H

#include "AdVKCommon.h"
#include <vulkan/vulkan_core.h>


namespace ade {
  class AdVKDevice;

  class AdVKCommandPool{
    public:
      AdVKCommandPool(AdVKDevice *device, uint32_t queueFamilyIndex);
      ~AdVKCommandPool();

      static void BeginCommandBuffer(VkCommandBuffer cmdBuffer);
      static void EndCommandBuffer(VkCommandBuffer cmdBuffer);

      std::vector<VkCommandBuffer> AllocateCommandBuffer (uint32_t count) const;

      VkCommandPool GetHandle() const {return mHandle;}
    private:
      VkCommandPool mHandle;

      AdVKDevice *mDevice;
  };

}

#endif