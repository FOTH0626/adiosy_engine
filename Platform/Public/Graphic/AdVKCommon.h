#ifndef AD_VK_COMMON_H
#define AD_VK_COMMON_H

#include "AdLog.h"
#include <cstdint>
#include <cstring>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

struct DeviceFeature{
    const char* name;
    bool required;
};

#define CALL_VK(func) check_vk_result(func, __FILE_NAME__, __LINE__, #func)

/**
 * check device features
 * @param label               name
 * @param bExtension          layer or extension
 * @param availableCount      available count
 * @param available           available
 * @param requestedCount      requested count
 * @param requestFeatures     request features
 * @param outEnableCount      out enable count
 * @param outEnableFeatures   out enable features
 * @return                    is found all required features
 */

static bool checkDeviceFeatures(const char* label, bool bExtension, uint32_t availableCount, void *available,
                                uint32_t requestedCount, const DeviceFeature *requestFeatures,
                                uint32_t *outEnableCount, const char *outEnableFeatures[]){
          bool bFoundAllFeatures = true;

          *outEnableCount = 0;


          LOG_D("---------------------");
          LOG_D("{0}",label);
          for (int i = 0; i < requestedCount; ++i) {
            bool bFound = false;
            const char*  result = requestFeatures[i].required ? "(required not found)":"(not found)";
            for (int j =0; j < availableCount; ++j) {
              const char* availableName = bExtension ? 
              ((VkExtensionProperties*)(available))[j].extensionName :
              ((VkLayerProperties*)(available))[j].layerName;
              if(strcmp(availableName, requestFeatures[i].name) ==0 ){
                outEnableFeatures[(*outEnableCount)++] = availableName;
                bFound = true;
                break;
              }
            }

            if (bFound){
              result = requestFeatures[i].required ? "(required ,enable)":"(enable)";
            }

            bFoundAllFeatures &= bFound || !requestFeatures[i].required;

            LOG_D("{0} {1}", requestFeatures[i].name,result);
          }
          LOG_D("-------------------------------");
          return bFoundAllFeatures;          
}

#define ENUM_TO_STR(r)   \
	case r: return #r

static const char *
vk_result_string(VkResult code){
    switch (code) {
        ENUM_TO_STR(VK_SUCCESS);
        ENUM_TO_STR(VK_NOT_READY);
        ENUM_TO_STR(VK_TIMEOUT);
        ENUM_TO_STR(VK_EVENT_SET);
        ENUM_TO_STR(VK_EVENT_RESET);
        ENUM_TO_STR(VK_INCOMPLETE);
        ENUM_TO_STR(VK_ERROR_OUT_OF_HOST_MEMORY);
        ENUM_TO_STR(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        ENUM_TO_STR(VK_ERROR_INITIALIZATION_FAILED);
        ENUM_TO_STR(VK_ERROR_DEVICE_LOST);
        ENUM_TO_STR(VK_ERROR_MEMORY_MAP_FAILED);
        ENUM_TO_STR(VK_ERROR_LAYER_NOT_PRESENT);
        ENUM_TO_STR(VK_ERROR_EXTENSION_NOT_PRESENT);
        ENUM_TO_STR(VK_ERROR_FEATURE_NOT_PRESENT);
        ENUM_TO_STR(VK_ERROR_INCOMPATIBLE_DRIVER);
        ENUM_TO_STR(VK_ERROR_TOO_MANY_OBJECTS);
        ENUM_TO_STR(VK_ERROR_FORMAT_NOT_SUPPORTED);
        ENUM_TO_STR(VK_ERROR_FRAGMENTED_POOL);
#ifdef VK_VERSION_1_1
        ENUM_TO_STR(VK_ERROR_OUT_OF_POOL_MEMORY);
        ENUM_TO_STR(VK_ERROR_INVALID_EXTERNAL_HANDLE);
#endif
#ifdef VK_VERSION_1_2
        ENUM_TO_STR(VK_ERROR_UNKNOWN); // Only defined in 1.2 and above headers.
        ENUM_TO_STR(VK_ERROR_FRAGMENTATION);
        ENUM_TO_STR(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
#else
        case -13 /* VK_ERROR_UNKNOWN */: return "VK_ERROR_UNKNOWN"; // Has no guard.
#endif
#ifdef VK_VERSION_1_3
        ENUM_TO_STR(VK_PIPELINE_COMPILE_REQUIRED);
#endif
#ifdef VK_KHR_surface
        ENUM_TO_STR(VK_ERROR_SURFACE_LOST_KHR);
        ENUM_TO_STR(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
#endif
#ifdef VK_KHR_swapchain
        ENUM_TO_STR(VK_SUBOPTIMAL_KHR);
        ENUM_TO_STR(VK_ERROR_OUT_OF_DATE_KHR);
#endif
#ifdef VK_KHR_display_swapchain
        ENUM_TO_STR(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
#endif
#ifdef VK_EXT_debug_report
        ENUM_TO_STR(VK_ERROR_VALIDATION_FAILED_EXT);
#endif
#ifdef VK_NV_glsl_shader
        ENUM_TO_STR(VK_ERROR_INVALID_SHADER_NV);
#endif
#if defined(VK_ENABLE_BETA_EXTENSIONS) && defined(VK_KHR_video_queue)
        ENUM_TO_STR(VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR);
		ENUM_TO_STR(VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR);
		ENUM_TO_STR(VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR);
		ENUM_TO_STR(VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR);
		ENUM_TO_STR(VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR);
		ENUM_TO_STR(VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR);
#endif
#ifdef VK_EXT_image_drm_format_modifier
        ENUM_TO_STR(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
#endif
#ifdef VK_KHR_global_priority
        ENUM_TO_STR(VK_ERROR_NOT_PERMITTED_KHR);
#endif
#ifdef VK_EXT_full_screen_exclusive
        ENUM_TO_STR(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
#endif
#ifdef VK_KHR_deferred_host_operations
        ENUM_TO_STR(VK_THREAD_IDLE_KHR);
#endif
#ifdef VK_KHR_deferred_host_operations
        ENUM_TO_STR(VK_THREAD_DONE_KHR);
#endif
#ifdef VK_KHR_deferred_host_operations
        ENUM_TO_STR(VK_OPERATION_DEFERRED_KHR);
#endif
#ifdef VK_KHR_deferred_host_operations
        ENUM_TO_STR(VK_OPERATION_NOT_DEFERRED_KHR);
#endif
#ifdef VK_EXT_image_compression_control
        ENUM_TO_STR(VK_ERROR_COMPRESSION_EXHAUSTED_EXT);
#endif
#if defined(VK_KHR_maintenance1) && !defined(VK_VERSION_1_1)
        ENUM_TO_STR(VK_ERROR_OUT_OF_POOL_MEMORY_KHR);
#endif
#if defined(VK_KHR_external_memory) && !defined(VK_VERSION_1_1)
        ENUM_TO_STR(VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR);
#endif
#if defined(VK_EXT_descriptor_indexing) && !defined(VK_VERSION_1_2)
        ENUM_TO_STR(VK_ERROR_FRAGMENTATION_EXT);
#endif
#if defined(VK_EXT_global_priority) && !defined(VK_KHR_global_priority)
        ENUM_TO_STR(VK_ERROR_NOT_PERMITTED_EXT);
#endif
#if defined(VK_EXT_buffer_device_address) && !defined(VK_VERSION_1_2)
        ENUM_TO_STR(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT);
		// VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR = VK_ERROR_INVALID_DEVICE_ADDRESS_EXT
#endif
#if defined(VK_EXT_pipeline_creation_cache_control) && !defined(VK_VERSION_1_3)
        ENUM_TO_STR(VK_PIPELINE_COMPILE_REQUIRED_EXT);
		// VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT = VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT
#endif
        default: return "UNKNOWN RESULT";
    }
}

static void check_vk_result(VkResult result, const char *fileName, uint32_t line, const char* func){
    if(result != VK_SUCCESS){
        LOG_E("[{0}:{1}]: {2} ---> {3}", fileName, line, func, vk_result_string(result));
    }
}



#endif