#pragma once

#include <string_view>
#include <chrono>
#include <optional>
#include <iostream>
#include <optional>
#include <fstream>
#include <thread>

#include <nlohmann/json.hpp>


namespace RemoteGamepad
{
    namespace
    {
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

        namespace ConfigFields
        {
            const std::string_view RemoteAddress = "remote_address";
            const std::string_view RemotePort = "remote_port";
        }
    }
}
