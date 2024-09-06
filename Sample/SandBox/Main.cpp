
#include <iostream>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "AdLog.h"
#include "AdWindow.h"
#include "AdGraphicContext.h"
#include "Graphic/AdVKDevice.h"
#include "Graphic/AdVKFrameBuffer.h"
#include "Graphic/AdVKGraphicContext.h"
#include "Graphic/AdVKRenderPass.h"
#include "Graphic/AdVkSwapChain.h"

int main(){
    std::cout << "Hello adiosy engine."<< std::endl;

    ade::AdLog::Init();


    std::unique_ptr<ade::AdWindow> window = ade::AdWindow::Create(800, 600, "SandBox");
    std::unique_ptr<ade::AdGraphicContext> graphicContext = ade::AdGraphicContext::Create(window.get());
    auto vkContext = dynamic_cast<ade::AdVKGraphicContext*>(graphicContext.get());
    std::shared_ptr<ade::AdVKDevice> device = std::make_shared<ade::AdVKDevice>(vkContext, 1, 1);
    std::shared_ptr<ade::AdVKSwapChain> swapchain = std::make_shared<ade::AdVKSwapChain>(vkContext, device.get());
    swapchain->ReCreate();
    std::shared_ptr<ade::AdVKRenderPass> renderPass = std::make_shared<ade::AdVKRenderPass>(device.get()); 


    std::vector<VkImage> swapchainImages = swapchain->GetImages();
    std::vector<std::shared_ptr<ade::AdVKFrameBuffer>> framebuffers;
    for (const auto &image  : swapchainImages) {
        std::vector<VkImage> images = {image};
        framebuffers.push_back(std::make_shared<ade::AdVKFrameBuffer>(device.get(), renderPass.get(), images, swapchain->GetWidth(),swapchain->GetHeight()));
    }

    while (!window->ShouldClose()) {
        window->PollEvents();
        window->SwapBuffer();
    
    }


    return EXIT_SUCCESS;
}