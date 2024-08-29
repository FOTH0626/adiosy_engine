#include "Graphic/AdVKGraphicContext.h"
#include "AdLog.h"
#include "AdWindow.h"
#include "GLFW/glfw3.h"
#include "Graphic/AdVKCommon.h"
#include "Window/AdGLFWwindow.h"
#include <unordered_set>




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
        {VK_EXT_DEBUG_REPORT_EXTENSION_NAME, true}
    };
    AdVKGraphicContext::AdVKGraphicContext(AdWindow *window){
        CreateInstance();
        CreateSurface(window);
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
        uint32_t availableLayerCount;
        CALL_VK(vkEnumerateInstanceLayerProperties(&availableLayerCount,nullptr));
        VkLayerProperties availableLayers[availableLayerCount];
        CALL_VK(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers));

        uint32_t enableLayerCount = 0;
        const char* enableLayers[32];
        if (bShouldValidate) {
            if(!checkDeviceFeatures("Instance Layers:", false, availableLayerCount, availableLayers, 
        ARRAY_SIZE(requestedLayers), requestedLayers, &enableLayerCount, enableLayers)){
                return;
            };
        }
        


        //构建拓展
        uint32_t availableExtensionCount;
        CALL_VK(vkEnumerateInstanceExtensionProperties("",&availableExtensionCount,nullptr));
        VkExtensionProperties availableExtensions[availableExtensionCount];
        CALL_VK(vkEnumerateInstanceExtensionProperties("",&availableExtensionCount, availableExtensions));

        uint32_t glfwRequestedExtensionCount;
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

        uint32_t enableExtensionCount;
        const char* enableExtensions[32];
        if(!checkDeviceFeatures("Instance Extension:", true, availableExtensionCount, availableExtensions, 
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
}