#include "Graphic/AdVKGraphicContext.h"
#include "AdLog.h"
#include "AdWindow.h"
#include "GLFW/glfw3.h"
#include "Graphic/AdVKCommon.h"
#include "Window/AdGLFWwindow.h"
#include <cmath>
#include <unordered_set>
#include <vector>
#include <vulkan/vulkan_core.h>




namespace ade {

    const DeviceFeature requestedLayers[] = {
        {"VK_LAYER_KHRONOS_validation", true},
    };
    const DeviceFeature requestedExtensions[ ] = {
        {VK_KHR_SURFACE_EXTENSION_NAME,true},
#ifdef AD_ENGINE_PLATFORM_WIN32
            { VK_KHR_WIN32_SURFACE_EXTENSION_NAME, true },
#elif AD_ENGINE_PLATFORM_MACOS
            { VK_MVK_MACOS_SURFACE_EXTENSION_NAME, true },
#elif AD_ENGINE_PLATFORM_LINUX
        { VK_KHR_XCB_SURFACE_EXTENSION_NAME, true },
#endif
        //添加验证
        {VK_EXT_DEBUG_REPORT_EXTENSION_NAME, true}
    };
    AdVKGraphicContext::AdVKGraphicContext(AdWindow *window){
        CreateInstance();
        CreateSurface(window);
        SelectPhyDevice();
    }

    AdVKGraphicContext::~AdVKGraphicContext(){
        vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
        vkDestroyInstance(mInstance, nullptr);
    }

    static VkBool32 VkDebugReportCallback(VkDebugReportFlagsEXT flags,
                                        VkDebugReportObjectTypeEXT objectType, 
                                        uint64_t objcet,
                                        size_t location,
                                        int32_t messageCode,
                                        const char* pLayerPrefix,
                                        const char* pMessage,
                                        void* pUserData){
        if(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) 
            LOG_E("{0}",pMessage);
        if(flags & VK_DEBUG_REPORT_WARNING_BIT_EXT || flags &VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) 
            LOG_W("{0}",pMessage);
        return VK_TRUE;
    }

    void AdVKGraphicContext::CreateInstance(){
        //获取layers
        std::uint32_t availableLayerCount;
        CALL_VK(vkEnumerateInstanceLayerProperties(&availableLayerCount,nullptr));
        std::vector<VkLayerProperties> availableLayers{availableLayerCount};
        CALL_VK(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data()));

        std::uint32_t enableLayerCount = 0;
        const char* enableLayers[32];
        if (bShouldValidate) {
            if(!checkDeviceFeatures("Instance Layers:", false, availableLayerCount, availableLayers.data(), 
        ARRAY_SIZE(requestedLayers), requestedLayers, &enableLayerCount, enableLayers)){
                return;
            };
        }
        

        //构建拓展
        std::uint32_t availableExtensionCount;
        CALL_VK(vkEnumerateInstanceExtensionProperties("",&availableExtensionCount,nullptr));
        std::vector<VkExtensionProperties> availableExtensions{availableExtensionCount};
        CALL_VK(vkEnumerateInstanceExtensionProperties("",&availableExtensionCount, availableExtensions.data()));

        std::uint32_t glfwRequestedExtensionCount;
        const char** glfwRequestedExtensions = glfwGetRequiredInstanceExtensions(&glfwRequestedExtensionCount);
        std::unordered_set<const char*> allRequestedExtensionSet;
        std::vector<DeviceFeature> allRequestedExtensions;
        for(const auto &item :requestedExtensions){
            if(allRequestedExtensionSet.find(item.name) == allRequestedExtensionSet.end()){
                allRequestedExtensionSet.insert(item.name);
                allRequestedExtensions.push_back(item);
            }
        }
        for (int i = 0 ;i < glfwRequestedExtensionCount; ++i) {
            const char *extensionName = glfwRequestedExtensions[i];
            if (allRequestedExtensionSet.find(extensionName) == allRequestedExtensionSet.end()) {
                allRequestedExtensionSet.insert(extensionName);
                allRequestedExtensions.push_back({extensionName,true});
            }
        }

        std::uint32_t enableExtensionCount;
        const char* enableExtensions[32];
        if(!checkDeviceFeatures("Instance Extension:", true, availableExtensionCount, availableExtensions.data(), 
        allRequestedExtensions.size(), allRequestedExtensions.data(), &enableExtensionCount, enableExtensions)){
            
            return;
        };
    
        //create instance
        VkApplicationInfo applicationInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "Adiosy_Engine",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "None",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_3
        };

        VkDebugReportCallbackCreateInfoEXT debugReportCallbackInfoExt = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = VK_DEBUG_REPORT_WARNING_BIT_EXT
                    | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT 
                    | VK_DEBUG_REPORT_ERROR_BIT_EXT,
            .pfnCallback = VkDebugReportCallback
        };

        VkInstanceCreateInfo instanceInfo = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = bShouldValidate? &debugReportCallbackInfoExt : nullptr,
            .flags = 0,
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = enableLayerCount,
            .ppEnabledLayerNames = enableLayerCount > 0 ? enableLayers : nullptr,
            .enabledExtensionCount = enableExtensionCount,
            .ppEnabledExtensionNames = enableExtensionCount > 0 ? enableExtensions : nullptr
        }; 
        CALL_VK(vkCreateInstance(&instanceInfo, nullptr, &mInstance));
        LOG_T("{0} : Instance : {1}", __FUNCTION__, (void*)mInstance);

    }

    void AdVKGraphicContext::CreateSurface(AdWindow* window){
        if (!window) {
            LOG_E("window is not exists.");
            return;
        }

        auto *glfWwindow = dynamic_cast<AdGLFWwindow*>(window);
        if (!glfWwindow) {
            LOG_E("this window is not a glfw window.");
            return;
        
        }

        CALL_VK(glfwCreateWindowSurface(mInstance, glfWwindow->GetWindowHandle(),nullptr,&mSurface));
        LOG_T("{0} : Surface : {1}",__FUNCTION__,(void*)mSurface);
    }

    void AdVKGraphicContext::SelectPhyDevice(){
        std::uint32_t phyDeviceCount;
        CALL_VK(vkEnumeratePhysicalDevices(mInstance,&phyDeviceCount,nullptr));
        std::vector<VkPhysicalDevice> phyDevices{phyDeviceCount};
        CALL_VK(vkEnumeratePhysicalDevices(mInstance, &phyDeviceCount, phyDevices.data()));

        std::uint32_t maxScore = 0;
        int32_t maxScorePhyDeviceIndex = -1;
        LOG_D("------------------------");
        LOG_D("Physical devices: ");
        for (int i = 0 ; i < phyDeviceCount; ++i) {
            //log
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(phyDevices[i], &props);
            PrintPhyDeviceInfo(props);

            std::uint32_t score = GetPhyDeviceScore(props);
            std::uint32_t formatCount;            
            CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(phyDevices[i], mSurface, &formatCount, nullptr));
            std::vector<VkSurfaceFormatKHR> formats{formatCount};
            CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(phyDevices[i], mSurface, &formatCount, formats.data()));
            for (int j = 0 ; j < formatCount; ++j) {
              if (formats[j].format == VK_FORMAT_B8G8R8A8_UNORM && formats[j].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
                  score +=10;
                  break;
              }
            }

            

            std::uint32_t queueFamilyCount;
            vkGetPhysicalDeviceQueueFamilyProperties(phyDevices[i], &queueFamilyCount, nullptr);
            std::vector<VkQueueFamilyProperties> queueFamilys{queueFamilyCount};
            vkGetPhysicalDeviceQueueFamilyProperties(phyDevices[i], &queueFamilyCount, queueFamilys.data());
            
            LOG_D("score ---->   :{0}",score);
            LOG_D("queue family  :{0}",queueFamilyCount);
            if (score < maxScore) {
              continue;
            }
            
            
            for(int j = 0; j < queueFamilyCount; ++j){
              if (queueFamilys[j].queueCount == 0) {
                continue;
              }

            //1.graphic family
              if (queueFamilys[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                mGraphicQueueFamily.queueFamilyIndex = j;
                mGraphicQueueFamily.queueFamilyCount = queueFamilys[j].queueCount;
              }

              if (mGraphicQueueFamily.queueFamilyIndex >= 0 && mPresentQueueFamily.queueFamilyIndex >= 0
              && mGraphicQueueFamily.queueFamilyIndex != mPresentQueueFamily.queueFamilyIndex) {
                break;  
              }
            //2.present family
              VkBool32 bSupportSurface;
              vkGetPhysicalDeviceSurfaceSupportKHR(phyDevices[i], j, mSurface, &bSupportSurface);
              if(bSupportSurface){
                mPresentQueueFamily.queueFamilyIndex = j;
                mPresentQueueFamily.queueFamilyCount = queueFamilys[j].queueCount;
              }
            }

          
          if (mGraphicQueueFamily.queueFamilyIndex >=0 && mPresentQueueFamily.queueFamilyIndex >=0) {
            maxScorePhyDeviceIndex = i ;
            maxScore = score;
          }
        }
        LOG_D("------------------------");

        
        if (maxScorePhyDeviceIndex < 0) {
          LOG_W("Maybe can not find a suitable physical device ,will 0.");
          maxScorePhyDeviceIndex = 0;
        }

        mPhyDevice = phyDevices[maxScorePhyDeviceIndex];
        vkGetPhysicalDeviceMemoryProperties(mPhyDevice, &mPhyDeviceMemoryProperties);
        LOG_T("{0} : physical device:{1},score:{2}, graphic queue:{3}:{4},present queue:{5} : {6}",__FUNCTION__,
                maxScorePhyDeviceIndex,maxScore,
                mGraphicQueueFamily.queueFamilyIndex,mGraphicQueueFamily.queueFamilyCount,
                mPresentQueueFamily.queueFamilyIndex,mPresentQueueFamily.queueFamilyCount);
        
    }

    void AdVKGraphicContext::PrintPhyDeviceInfo(VkPhysicalDeviceProperties &props){

        const char *deviceType = props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? "intergrated gpu" :
                                  props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ?  "discrete gpu" :
                                  props.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU ?  "virtual gpu" : 
                                  props.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU ? "cpu" : "Others";

        std::uint32_t driverVersionMajor = VK_VERSION_MAJOR(props.driverVersion);
        std::uint32_t driverVersionMinor = VK_VERSION_MINOR(props.driverVersion);
        std::uint32_t driverVersionPatch = VK_VERSION_PATCH(props.driverVersion);

        std::uint32_t apiVersionMajor = VK_VERSION_MAJOR(props.apiVersion);
        std::uint32_t apiVersionMinor = VK_VERSION_MINOR(props.apiVersion);
        std::uint32_t apiVersionPatch = VK_VERSION_PATCH(props.apiVersion);
        

        LOG_D("------------------------");
        LOG_D("deviceName      : {0}",props.deviceName);
        LOG_D("deviceType      : {0}",deviceType);
        LOG_D("venderID        : {0}",props.vendorID);
        LOG_D("deviceID        : {0}",props.deviceID);
        LOG_D("driverVersion   : {0}.{1}.{2}",driverVersionMajor,driverVersionMinor,driverVersionPatch);
        LOG_D("apiVersion      : {0}.{1}.{2}",apiVersionMajor,apiVersionMinor,apiVersionPatch);
    }

    std::uint32_t AdVKGraphicContext::GetPhyDeviceScore(VkPhysicalDeviceProperties &props){
        VkPhysicalDeviceType deviceType = props.deviceType;
        std::uint32_t score = 0;
        switch (deviceType) {
            case VK_PHYSICAL_DEVICE_TYPE_OTHER:
              break;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
              score += 30;
              break;
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
              score += 40;
              break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
              score +=20;
              break;
            case VK_PHYSICAL_DEVICE_TYPE_CPU:
              score +=10;
              break;
            case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
              break;
        }

        return score;
    }
}