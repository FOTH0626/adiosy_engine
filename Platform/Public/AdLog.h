#ifndef ADLOG_H
#define ADLOG_H

#include "AdEngine.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "spdlog/spdlog.h"

namespace ade {
    class AdLog{
    public:
        AdLog() = delete;
        AdLog(const AdLog&) = delete;
        AdLog &operator=(const AdLog&) = delete;


        static void Init();
        static spdlog::logger* GetLoggerInstance(){
            assert(sLoggerInstance && "Logger instance is null, maybe you hace not execute AdLog::Init()");
            return sLoggerInstance.get();
        }
    private:
        static std::shared_ptr<spdlog::logger> sLoggerInstance;
    };

#define LOG_T(...) SPDLOG_LOGGER_TRACE(ade::AdLog::GetLoggerInstance(),__VA_ARGS__)
#define LOG_D(...) SPDLOG_LOGGER_DEBUG(ade::AdLog::GetLoggerInstance(),__VA_ARGS__)
#define LOG_I(...) SPDLOG_LOGGER_INFO(ade::AdLog::GetLoggerInstance(),__VA_ARGS__)
#define LOG_W(...) SPDLOG_LOGGER_WARN(ade::AdLog::GetLoggerInstance(),__VA_ARGS__)
#define LOG_E(...) SPDLOG_LOGGER_ERROR(ade::AdLog::GetLoggerInstance(),__VA_ARGS__)



}
#endif