
#include <iostream>
#include "AdLog.h"
#include "AdWindow.h"

int main(){
    std::cout << "Hello adiosy engine."<< std::endl;

    ade::AdLog::Init();
    //ade::AdLog::GetLoggerInstance()->debug("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    // LOG_T("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    // LOG_D("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    // LOG_I("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    // LOG_W("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    // LOG_E("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);

    std::unique_ptr<ade::AdWindow> window = ade::AdWindow::Create(800, 600, "SandBox");
    while (!window->ShouldClose()) {
        window->PollEvents();
        window->SwapBuffer();
    
    }


    return EXIT_SUCCESS;
}