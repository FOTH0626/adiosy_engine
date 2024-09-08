#include "AdFileUtil.h"
#include "AdLog.h"
#include "AdWindow.h"
#include "AdGraphicContext.h"
#include "Graphic/AdVKCommandBuffer.h"
#include "Graphic/AdVKDevice.h"
#include "Graphic/AdVKFrameBuffer.h"
#include "Graphic/AdVKGraphicContext.h"
#include "Graphic/AdVKPipeline.h"
#include "Graphic/AdVKQueue.h"
#include "Graphic/AdVKRenderPass.h"
#include "Graphic/AdVkSwapChain.h"
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>


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

    std::shared_ptr<ade::AdVKPipelineLayout> pipelineLayout = std::make_shared<ade::AdVKPipelineLayout>(device.get(),
                                                                                                        AD_RES_SHADER_DIR"00_hello_triangle.vert",
                                                                                                        AD_RES_SHADER_DIR"00_hello_triangle.frag");

    std::shared_ptr<ade::AdVKPipeline> pipeline = std::make_shared<ade::AdVKPipeline>(device.get(), renderPass.get(),pipelineLayout.get());
    pipeline->SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)->EnableDepthTest();
    pipeline->SetDynamicState({VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR});
    pipeline->Create();

    std::shared_ptr<ade::AdVKCommandPool> cmdPool = std::make_shared<ade::AdVKCommandPool>(device.get(), vkContext->GetGraphicQueueFamilyInfo().queueFamilyIndex);
    std::vector<VkCommandBuffer> cmdBuffers = cmdPool->AllocateCommandBuffer(swapchainImages.size());

    ade::AdVKQueue *graphicQueue = device->GetFirstGraphicQueue();

    const std::vector<VkClearValue> clearValues = {
      {0.1f,.2f,.3f,1.f}
    };

    while (!window->ShouldClose()) {
        window->PollEvents();

        //acquire swapchain image
        int32_t imageIndex = swapchain->AcquireImage();
        //begin cmdbuffer
        ade::AdVKCommandPool::BeginCommandBuffer(cmdBuffers[imageIndex]);
        //begin renderpass, bind framebuffer
        renderPass->Begin(cmdBuffers[imageIndex], framebuffers[imageIndex].get(), clearValues);
        //bind resource pipelin, geometry, descriptorset
        pipeline->Bind(cmdBuffers[imageIndex]);

        VkViewport viewport = {
            .x = 0,
            .y = 0,
            .width = static_cast<float>(framebuffers[imageIndex]->GetWidth()),
            .height = static_cast<float>(framebuffers[imageIndex]->GetHeight()),
        };
        vkCmdSetViewport(cmdBuffers[imageIndex], 0, 1, &viewport);

        VkRect2D scissor = {
            .offset = {0, 0},
            .extent = {framebuffers[imageIndex]->GetWidth(),framebuffers[imageIndex]->GetHeight()}
        };
        vkCmdSetScissor(cmdBuffers[imageIndex], 0, 1, &scissor);
        //draw
        vkCmdDraw(cmdBuffers[imageIndex], 3, 1, 0, 0);
        //end renderpass
        renderPass->End(cmdBuffers[imageIndex]);
        //end cmdbuffer
        ade::AdVKCommandPool::EndCommandBuffer(cmdBuffers[imageIndex]);
        //submit cmdbuffer to queue
        graphicQueue->Submit({cmdBuffers[imageIndex]});
        graphicQueue->WaitIdle();
        //present
        swapchain->Present(imageIndex);


        window->SwapBuffer();
    
    }


    return EXIT_SUCCESS;
}