#include "Graphic/AdVKQueue.h"
#include "AdLog.h"
#include "Graphic/AdVKCommon.h"
#include <vulkan/vulkan_core.h>

namespace ade {
  AdVKQueue::AdVKQueue(std::uint32_t familyIndex, std::uint32_t index, VkQueue queue, bool canPresent)
      :mFamilyIndex(familyIndex), mIndex(index), mQueue(queue), canPresent(canPresent){
        LOG_T("Create a new queue: {0} - {1} - {2}, present {3}",mFamilyIndex, index, (void*)queue, canPresent);
  }

  void AdVKQueue::WaitIdle() const {
    CALL_VK(vkQueueWaitIdle(mQueue));
  }

}