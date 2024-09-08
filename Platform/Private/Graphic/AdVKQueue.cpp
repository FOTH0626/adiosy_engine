#include "Graphic/AdVKQueue.h"
#include "AdLog.h"
#include "Graphic/AdVKCommon.h"

namespace ade {
  AdVKQueue::AdVKQueue(std::uint32_t familyIndex, std::uint32_t index, VkQueue queue, bool canPresent)
      :mFamilyIndex(familyIndex), mIndex(index), mHandle(queue), canPresent(canPresent){
        LOG_T("Create a new queue: {0} - {1} - {2}, present {3}",mFamilyIndex, index, (void*)queue, canPresent);
  }

  void AdVKQueue::WaitIdle() const {
    CALL_VK(vkQueueWaitIdle(mHandle));
  }

  void AdVKQueue::Submit(std::vector<VkCommandBuffer> cmdBuffers){
    VkPipelineStageFlags waitDstStageMask[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = nullptr,
        .pWaitDstStageMask = waitDstStageMask,
        .commandBufferCount = static_cast<uint32_t>(cmdBuffers.size()),
        .pCommandBuffers = cmdBuffers.data(),
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = nullptr
    };
    CALL_VK(vkQueueSubmit(mHandle, 1, &submitInfo, VK_NULL_HANDLE));


  }
}