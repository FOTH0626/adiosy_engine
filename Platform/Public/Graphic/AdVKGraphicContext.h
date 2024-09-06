#ifndef AD_VK_GRAPHIC_CONTEXT_H
#define AD_VK_GRAPHIC_CONTEXT_H

#include "AdGraphicContext.h"
#include "AdWindow.h"
#include <vulkan/vulkan_core.h>

namespace ade {
    struct QueueFamilyInfo{
        int32_t queueFamilyIndex = -1;
        std::uint32_t queueFamilyCount;
    };

    class AdVKGraphicContext:public AdGraphicContext{
    public:
        AdVKGraphicContext(AdWindow* window);
        ~AdVKGraphicContext() override;

        VkInstance GetInstance() const {return mInstance;}
        VkSurfaceKHR GetSurface() const {return mSurface;}
        VkPhysicalDevice GetPhyDevice() const {return mPhyDevice;}
        const QueueFamilyInfo &GetGraphicQueueFamilyInfo() const {return mGraphicQueueFamily;}
        const QueueFamilyInfo &GetPresentQueueFamilyInfo() const {return mPresentQueueFamily;}
        VkPhysicalDeviceMemoryProperties GetPhyDeviceMemProperties() const {return mPhyDeviceMemoryProperties;}
        bool IsSameGraphicPresentQueueFamily() const {return mGraphicQueueFamily.queueFamilyIndex == mPresentQueueFamily.queueFamilyIndex; }

    private:
        static void PrintPhyDeviceInfo(VkPhysicalDeviceProperties &props);
        static std::uint32_t GetPhyDeviceScore(VkPhysicalDeviceProperties &props);

        void CreateInstance();
        void CreateSurface(AdWindow *window);
        void SelectPhyDevice();

        VkInstance mInstance;
        VkSurfaceKHR mSurface;
        
        bool bShouldValidate = true ;


        VkPhysicalDevice mPhyDevice;
        VkPhysicalDeviceMemoryProperties mPhyDeviceMemoryProperties;

        QueueFamilyInfo mGraphicQueueFamily;
        QueueFamilyInfo mPresentQueueFamily;

    };
}

#endif