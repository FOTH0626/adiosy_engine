#include "AdWindow.h"
#include "Window/AdGLFWwindow.h"
#include <cstdint>


namespace ade {
    std::unique_ptr<AdWindow> AdWindow::Create(std::uint32_t width,std::uint32_t height,const char* title){
#ifdef AD_ENGINE_PLATFORM_WIN32
    return std::make_unique<AdGLFWwindow>(width,height,title);
#elif AD_ENGINE_PLATFORM_MACOS
    return std::make_unique<AdGLFWwindow>(width,height,title);
#elif AD_ENGINE_PLATFORM_LINUX
    return std::make_unique<AdGLFWwindow>(width,height,title);
#endif
    return nullptr;
    }
}