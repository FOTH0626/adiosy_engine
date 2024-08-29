#ifndef AD_VK_GRAPHIC_CONTEXT_H
#define AD_VK_GRAPHIC_CONTEXT_H

#include "AdGraphicContext.h"
#include "AdVKCommon.h"
#include "AdWindow.h"

namespace ade {
    class AdVKGraphicContext:public AdGraphicContext{
    public:
        AdVKGraphicContext(AdWindow* window);
        ~AdVKGraphicContext() override;
    private:
        void CreateInstance();
        void CreateSurface(AdWindow *window);

        VkInstance mInstance;
        VkSurfaceKHR mSurface;
        
        bool bShouldValidate = true ;
    };
}

#endif