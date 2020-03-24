#pragma once

#include <string_view>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>


namespace RemoteGamepad
{
    namespace Logging
    {
        namespace
        {
            const std::string_view StdOutName = "stdout";
            const std::string_view StdErrName = "stderr";

            void setUpDefaultGlobalLoggers()
            {
                const auto stdoutLogger = spdlog::stdout_color_mt(StdOutName.data());
                const auto stderrLogger = spdlog::stderr_color_mt(StdErrName.data());

#ifdef _DEBUG
                stdoutLogger->set_level(spdlog::level::debug);
                stderrLogger->set_level(spdlog::level::debug);
#else
                stdoutLogger->set_level(spdlog::level::warn);
                stderrLogger->set_level(spdlog::level::warn);
#endif
            }

            auto StdOut() { return spdlog::get(StdOutName.data()); }
            auto StdErr() { return spdlog::get(StdErrName.data()); }
        }
    }
}
