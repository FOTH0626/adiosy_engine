#ifndef AD_GLFWWINDOW_H
#define AD_GLFWWINDOW_H

#include "AdWindow.h"
#include "GLFW/glfw3.h"


namespace ade {
    class AdGLFWwindow : public AdWindow{
    public:
        AdGLFWwindow() = delete;
        AdGLFWwindow(uint32_t width, uint32_t height, const char *title);
        ~AdGLFWwindow() override;
        
        bool ShouldClose() override;
        void PollEvents() override;
        void SwapBuffer() override;

        GLFWwindow *GetWindowHandle() const {return mGLFWwindow;}
    private:
        GLFWwindow *mGLFWwindow;
    };
}

#endif