#ifndef AD_WINDOW_H
#define AD_WINDOW_H

#include "AdEngine.h"

namespace ade {
    class AdWindow{
    public:
        AdWindow(const AdWindow&) = delete;
        AdWindow &operator=(const AdWindow&) = delete;
        virtual ~AdWindow() = default;

        static std::unique_ptr<AdWindow> Create(uint32_t width,uint32_t height, const char* title);

        virtual bool ShouldClose() =0 ;
        virtual void PollEvents() =0 ;
        virtual void SwapBuffer() =0 ;
    protected:
        AdWindow() = default;
    };
}



#endif