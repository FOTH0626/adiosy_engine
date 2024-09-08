#ifndef AD_VK_QUEUE_H
#define AD_VK_QUEUE_H

#include "AdVKCommon.h"




namespace ade {
  class AdVKQueue {
    public:
      AdVKQueue(std::uint32_t familyIndex, std::uint32_t index, VkQueue queue, bool canPresent);
      ~AdVKQueue() = default;

      void WaitIdle() const;
      VkQueue GetHandle() const {return mHandle;}

      void Submit(std::vector<VkCommandBuffer> cmdBuffers);
    private:
      std::uint32_t mFamilyIndex;
      std::uint32_t mIndex;
      VkQueue mHandle;
      bool canPresent;
  };
}

#endif