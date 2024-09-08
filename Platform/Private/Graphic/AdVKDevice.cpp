#include "Graphic/AdVKDevice.h"
#include "AdEngine.h"
#include "AdLog.h"
#include "Graphic/AdVKCommon.h"
#include "Graphic/AdVKGraphicContext.h"
#include "Graphic/AdVKQueue.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace ade {

    const DeviceFeature requestedExtensions[] = {
  {VK_KHR_SWAPCHAIN_EXTENSION_NAME,true},
#ifdef AD_ENGINE_PLATFORM_WIN32
#elif AD_ENGINE_PLATFORM_MACOS
#elif AD_ENGINE_PLATFORM_LINUX
#endif
    };    
    
    AdVKDevice::AdVKDevice(AdVKGraphicContext *context, std::uint32_t graphicQueueCount, std::uint32_t presentQueueCount,
                            const AdVkSettings &settings):mSettings(settings){
      if(!context){
        LOG_E("Must create a vulkan grahpic context before create device.");
        return;
      }

      QueueFamilyInfo graphicQueueFamilyInfo = context->GetGraphicQueueFamilyInfo();
      QueueFamilyInfo presentQueueFamilyInfo = context->GetPresentQueueFamilyInfo();
      if (graphicQueueCount > graphicQueueFamilyInfo.queueFamilyCount) {
        LOG_E("this queue family has {0} queue, but request {1}",graphicQueueFamilyInfo.queueFamilyCount, graphicQueueCount);
        return;
      }

      if (presentQueueCount> presentQueueFamilyInfo.queueFamilyCount) {
        LOG_E("this queue family has {0} queue, but request {1}",presentQueueFamilyInfo.queueFamilyCount, presentQueueCount);
        return;
      }

      std::vector<float> graphicQueuePriorities(graphicQueueCount, 0.f);
      std::vector<float> presentQueuePriorities(graphicQueueCount, 1.f);

      bool bSameQueueFamilyIndex = context->IsSameGraphicPresentQueueFamily();

      std::uint32_t sameQueueCount = graphicQueueCount;
      if (bSameQueueFamilyIndex) {
        sameQueueCount += presentQueueCount;
        if (sameQueueCount < graphicQueueFamilyInfo.queueFamilyCount) {
          sameQueueCount = graphicQueueFamilyInfo.queueFamilyCount;
        }
        graphicQueuePriorities.insert(graphicQueuePriorities.end(),presentQueuePriorities.begin(),presentQueuePriorities.end()); 
      }
    // VkStructureType             sType;
    // const void*                 pNext;
    // VkDeviceQueueCreateFlags    flags;
    // std::uint32_t                    queueFamilyIndex;
    // std::uint32_t                    queueCount;
    // const float*                pQueuePriorities;
      VkDeviceQueueCreateInfo queueInfos[2] = {
    { .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = static_cast<std::uint32_t>(graphicQueueFamilyInfo.queueFamilyIndex),
        .queueCount = sameQueueCount,
        .pQueuePriorities = graphicQueuePriorities.data()
        }
      };

      if (!bSameQueueFamilyIndex) {
        queueInfos[1] = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = static_cast<std::uint32_t>(presentQueueFamilyInfo.queueFamilyIndex),
        .queueCount = presentQueueCount,
        .pQueuePriorities = presentQueuePriorities.data()
        };
      }
    // VkStructureType                    sType;
    // const void*                        pNext;
    // VkDeviceCreateFlags                flags;
    // std::uint32_t                           queueCreateInfoCount;
    // const VkDeviceQueueCreateInfo*     pQueueCreateInfos;
    // std::uint32_t                           enabledLayerCount;
    // const char* const*                 ppEnabledLayerNames;
    // std::uint32_t                           enabledExtensionCount;
    // const char* const*                 ppEnabledExtensionNames;
    // const VkPhysicalDeviceFeatures*    pEnabledFeatures;

        std::uint32_t availableExtensionCount;
        CALL_VK(vkEnumerateDeviceExtensionProperties(context->GetPhyDevice(), "", &availableExtensionCount, nullptr));
        VkExtensionProperties availableExtensions[availableExtensionCount];
        CALL_VK(vkEnumerateDeviceExtensionProperties(context->GetPhyDevice(),"",&availableExtensionCount,availableExtensions));
        std::uint32_t enableExtensionCount;
        
        
        const char* enableExtensions[32];
        if(!checkDeviceFeatures("Device Extension:", true, availableExtensionCount, availableExtensions, 
        ARRAY_SIZE(requestedExtensions), requestedExtensions, &enableExtensionCount, enableExtensions)){
            LOG_W("unable check device features.");
            return;
        };


      VkDeviceCreateInfo deviceInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = static_cast<std::uint32_t>(bSameQueueFamilyIndex ? 1:2),
        .pQueueCreateInfos = queueInfos,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = enableExtensionCount,
        .ppEnabledExtensionNames = enableExtensionCount > 0 ? enableExtensions : nullptr,
        .pEnabledFeatures = nullptr
      };
      CALL_VK(vkCreateDevice(context->GetPhyDevice(), &deviceInfo, nullptr, &mHandle));
      LOG_T("VkDevice: {0}",(void*)mHandle);


      for (int i =0 ; i < graphicQueueCount; ++i) {
        VkQueue queue;
        vkGetDeviceQueue(mHandle,graphicQueueFamilyInfo.queueFamilyIndex,i, &queue);
        mGraphicQueues.push_back(std::make_shared<AdVKQueue>(graphicQueueFamilyInfo.queueFamilyIndex, i, queue, false));
      }

      for (int i =0 ; i < presentQueueCount; ++i) {
        VkQueue queue;
        vkGetDeviceQueue(mHandle,presentQueueFamilyInfo.queueFamilyIndex,i, &queue);
        mPresentQueues.push_back(std::make_shared<AdVKQueue>(presentQueueFamilyInfo.queueFamilyIndex, i, queue, true));
      }

      CreatePipelineCache();
    }
    AdVKDevice::~AdVKDevice(){
        vkDeviceWaitIdle(mHandle);
        VK_D(PipelineCache, mHandle, mPipelineCache);
        vkDestroyDevice(mHandle, nullptr);
    }

    void AdVKDevice::CreatePipelineCache() {
      VkPipelineCacheCreateInfo pipelineCacheInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0
      };
      CALL_VK(vkCreatePipelineCache(mHandle, &pipelineCacheInfo, nullptr, &mPipelineCache));
    }
}