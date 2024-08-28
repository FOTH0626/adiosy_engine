#include "Window/AdGLFWwindow.h"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "AdLog.h"
#include <GL/gl.h>

namespace ade{
    AdGLFWwindow::AdGLFWwindow(uint32_t width, uint32_t height, const char *title){
        if(!glfwInit()){
            LOG_E("Failed to init glfw.");
            return;
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

        mGLFWwindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if(!mGLFWwindow){
            LOG_E("Failed to cerate glfw window.");
            return;
        }

        GLFWmonitor* primaryMonitor =glfwGetPrimaryMonitor();

        if (primaryMonitor) {
            int xPos,yPos,workWidth,workHeight;
            glfwGetMonitorWorkarea(primaryMonitor, &xPos, &yPos, &workWidth, &workHeight);
            glfwSetWindowPos(mGLFWwindow, workWidth/2 - width/2, workHeight/2 - height/2);
        }
            glfwMakeContextCurrent(mGLFWwindow); 

            glfwShowWindow(mGLFWwindow);
    }
    
    AdGLFWwindow::~AdGLFWwindow(){
        glfwDestroyWindow(mGLFWwindow);
        glfwTerminate();
        LOG_I("The application running end.");
    }
    
    bool AdGLFWwindow::ShouldClose(){
        return glfwWindowShouldClose(mGLFWwindow);
    }

    void AdGLFWwindow::PollEvents(){
        glfwPollEvents();
    }

    void AdGLFWwindow::SwapBuffer(){
        glfwSwapBuffers(mGLFWwindow);
    }
}