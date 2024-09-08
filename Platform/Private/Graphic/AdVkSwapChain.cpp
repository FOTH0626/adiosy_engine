#include "Graphic/AdVkSwapChain.h" 
#include "AdLog.h"
#include "Graphic/AdVKCommon.h"
#include "Graphic/AdVKDevice.h"
#include "Graphic/AdVKGraphicContext.h"
#include "Graphic/AdVKQueue.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace ade {
  AdVKSwapChain::AdVKSwapChain(AdVKGraphicContext *context, AdVKDevice *device):mContext(context), mDevice(device){
    ReCreate();
  }

  AdVKSwapChain::~AdVKSwapChain(){
    VK_D(SwapchainKHR, mDevice->GetHandle(), mHandle);
    mHandle = VK_NULL_HANDLE;
  }

  bool AdVKSwapChain::ReCreate(){
    VK_D(SwapchainKHR, mDevice->GetHandle(), mHandle);
    mHandle = VK_NULL_HANDLE;

    SetupSurfaceCapabilities();
    LOG_D("currentExtent : {0} x {1}",mSurfaceInfo.capabilities.currentExtent.width,mSurfaceInfo.capabilities.currentExtent.height);
    LOG_D("surfaceFormat : {0}", vk_format_string(mSurfaceInfo.surfaceFormat.format));
    LOG_D("presentMOde : {}",vk_present_mode_string(mSurfaceInfo.presentMode));
    LOG_D("-----------------------------------");

    std::uint32_t imageCount = mDevice->GetSettings().swapchainImageCount;
    if (imageCount < mSurfaceInfo.capabilities.minImageCount && mSurfaceInfo.capabilities.minImageCount > 0) {
      imageCount = mSurfaceInfo.capabilities.minImageCount;
    }
    if (imageCount > mSurfaceInfo.capabilities.maxImageCount && mSurfaceInfo.capabilities.maxImageCount > 0) {
      imageCount = mSurfaceInfo.capabilities.maxImageCount;
    }

    VkSharingMode imageSharingMode;
    std::uint32_t queueFamilyIndexCount;
    std::uint32_t pQueueFamilyIndices[2] = {0,0};
    if (mContext->IsSameGraphicPresentQueueFamily()) {
      imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      queueFamilyIndexCount = 0;
    }else{
      imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      queueFamilyIndexCount = 2;
      pQueueFamilyIndices[0] = mContext->GetGraphicQueueFamilyInfo().queueFamilyIndex;
      pQueueFamilyIndices[1] = mContext->GetPresentQueueFamilyInfo().queueFamilyIndex;
    }
    // VkStructureType                  sType;
    // const void*                      pNext;
    // VkSwapchainCreateFlagsKHR        flags;
    // VkSurfaceKHR                     surface;
    // uint32_t                         minImageCount;
    // VkFormat                         imageFormat;
    // VkColorSpaceKHR                  imageColorSpace;
    // VkExtent2D                       imageExtent;
    // uint32_t                         imageArrayLayers;
    // VkImageUsageFlags                imageUsage;
    // VkSharingMode                    imageSharingMode;
    // uint32_t                         queueFamilyIndexCount;
    // const uint32_t*                  pQueueFamilyIndices;
    // VkSurfaceTransformFlagBitsKHR    preTransform;
    // VkCompositeAlphaFlagBitsKHR      compositeAlpha;
    // VkPresentModeKHR                 presentMode;
    // VkBool32                         clipped;
    // VkSwapchainKHR                   oldSwapchain;
    VkSwapchainKHR oldSwapchain = mHandle;

    VkSwapchainCreateInfoKHR swapchainInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = mContext->GetSurface(),
        .minImageCount = imageCount,
        .imageFormat = mSurfaceInfo.surfaceFormat.format,
        .imageColorSpace = mSurfaceInfo.surfaceFormat.colorSpace,
        .imageExtent = mSurfaceInfo.capabilities.currentExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = imageSharingMode,
        .queueFamilyIndexCount = queueFamilyIndexCount,
        .pQueueFamilyIndices = pQueueFamilyIndices,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
        .presentMode = mSurfaceInfo.presentMode,
        .clipped = VK_FALSE,
        .oldSwapchain = oldSwapchain
    };
    VkResult ret = vkCreateSwapchainKHR(mDevice->GetHandle(), &swapchainInfo,nullptr, &mHandle);
    if (ret != VK_SUCCESS) {
      LOG_E("{0} : {1}", __FUNCTION__,vk_result_string(ret));
      return false;
    }
    LOG_T("Swapchain {0} : old: {1}, new: {2}, image count: {3}, format: {4}, present mode : {5}",__FUNCTION__,(void*)oldSwapchain, (void*)mHandle,
      imageCount, vk_format_string(mSurfaceInfo.surfaceFormat.format), vk_present_mode_string(mSurfaceInfo.presentMode));
    std::uint32_t swapchainImageCount;
    ret = vkGetSwapchainImagesKHR(mDevice->GetHandle(), mHandle,  &swapchainImageCount, nullptr);
    mImages.resize(swapchainImageCount);
    ret = vkGetSwapchainImagesKHR(mDevice->GetHandle(), mHandle,  &swapchainImageCount, mImages.data());
    return ret == VK_SUCCESS;
  }

  void AdVKSwapChain::SetupSurfaceCapabilities(){
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mContext->GetPhyDevice(),mContext->GetSurface(),&mSurfaceInfo.capabilities);

    AdVkSettings settings = mDevice->GetSettings();

    std::uint32_t formatCount;
    CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(mContext->GetPhyDevice(), mContext->GetSurface(), &formatCount, nullptr));
    if (formatCount == 0) {
      LOG_E("{0}: num of surface format is 0",__FUNCTION__);
      return;
    }

    std::vector<VkSurfaceFormatKHR> formats;
    formats.resize(formatCount);
    CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(mContext->GetPhyDevice(), mContext->GetSurface(), &formatCount, formats.data()));
    int32_t foundFormatIndex = -1;
    for ( int i = 0; i < formatCount; ++i) {
      if (formats[i].format == settings.surfaceFormat && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
        foundFormatIndex =i;
        break;
      }
    }
    if (foundFormatIndex == -1) {
      foundFormatIndex = 0;
    }
    mSurfaceInfo.surfaceFormat = formats.at(foundFormatIndex);

    //present mode 

    std::uint32_t presentModeCount;
    CALL_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(mContext->GetPhyDevice(), mContext->GetSurface(), &presentModeCount, nullptr));
    if (presentModeCount == 0) {
      LOG_E("{0}: num of surface format is 0",__FUNCTION__);
      return;
    }

    std::vector<VkPresentModeKHR> presentModes;
    presentModes.resize(presentModeCount);
    CALL_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(mContext->GetPhyDevice(), mContext->GetSurface(), &presentModeCount, presentModes.data()));
    VkPresentModeKHR preferredPresentMode = mDevice->GetSettings().presentMode;
    int32_t foundPresentModeIndex = -1;
    for ( int i = 0; i < presentModeCount; ++i) {
      if (presentModes.at(i) == preferredPresentMode){
        foundPresentModeIndex =i;
        break;
      }
    }
    if (foundPresentModeIndex >= 0) {
      mSurfaceInfo.presentMode = presentModes.at(foundPresentModeIndex);
    }else {
      mSurfaceInfo.presentMode = presentModes.at(0);

    }

  }  

  int32_t AdVKSwapChain::AcquireImage() const {
    uint32_t imageIndex;
    CALL_VK(vkAcquireNextImageKHR(mDevice->GetHandle(), mHandle, UINT64_MAX, VK_NULL_HANDLE, VK_NULL_HANDLE, &imageIndex));
    return imageIndex;
  }

  void AdVKSwapChain::Present(int32_t imageIndex) const{
    VkPresentInfoKHR presentInfo = {
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext = nullptr,
      .waitSemaphoreCount = 0,
      .pWaitSemaphores = nullptr,
      .swapchainCount = 1,
      .pSwapchains = &mHandle,
      .pImageIndices = reinterpret_cast<const uint32_t *>(&imageIndex)
    };
    CALL_VK(vkQueuePresentKHR(mDevice->GetFirstPresentQueue()->GetHandle(), &presentInfo));
    mDevice->GetFirstPresentQueue()->WaitIdle();
  }
}