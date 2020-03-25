#pragma once

#include <string_view>
#include <chrono>
#include <optional>
#include <iostream>
#include <optional>
#include <fstream>
#include <thread>
#include <sstream>
#include <future>

#include <nlohmann/json.hpp>


namespace RemoteGamepad
{
    using VersionType = std::string_view;

    namespace
    {
        constexpr VersionType Version = "1.0.2";

        const std::string_view ConfigFile = "config.json";


        std::optional<nlohmann::json> readConfigFile()
        {
            std::ifstream configFileStream(RemoteGamepad::ConfigFile.data(), std::ios::in);

            if (!configFileStream)
            {
                return std::nullopt;
            }

            nlohmann::json configJson;

            configFileStream >> configJson;

            return configJson;
        }

        template <class TValue>
        std::optional<TValue> readFromConfig(const nlohmann::json& config, std::string_view fieldName)
        {
            const auto it = config.find(fieldName);

            if (it == config.end())
            {
                return std::nullopt;
            }

            return it->get<TValue>();
        }

        void waitNextFrame()
        {
            const std::chrono::milliseconds FrameDuration(30);
            std::this_thread::sleep_for(FrameDuration);
        }

        template <class TFuture>
        bool futureReady(const TFuture& future)
        {
            return future.wait_for(std::chrono::microseconds(0)) == std::future_status::ready;
        }

        namespace ConfigFields
        {
            const std::string_view RemoteAddress = "remote_address";
            const std::string_view RemotePort = "remote_port";
            const std::string_view LeftThumbDeadzone = "left_thumb_deadzone";
            const std::string_view RightThumbDeadzone = "right_thumb_deadzone";
        }
    }
}
