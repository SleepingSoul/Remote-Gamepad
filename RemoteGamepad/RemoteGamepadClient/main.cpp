#include <cstdio>
#include <iostream>
#include <string_view>
#include <optional>
#include <fstream>
#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>

#include "RemoteGamepadClient.h"

namespace
{
    const std::string_view ConfigFile = "config.json";

    const std::chrono::milliseconds FrameDuration(15);

    template <class TValue>
    std::optional<TValue> readFromConfig(const nlohmann::json& config, std::string_view fieldName)
    {
        const auto it = config.find(fieldName);

        if (it == config.end())
        {
            std::printf("No '%s' provided in %s.", fieldName.data(), ConfigFile.data());
            return std::nullopt;
        }

        return it->get<TValue>();
    }
    
    namespace ConfigFields
    {
        const std::string_view RemoteAddress = "remote_address";
        const std::string_view RemotePort = "remote_port";
    }
}

int main()
{
    std::puts("Reading config file...");

    std::ifstream configFileStream(ConfigFile.data(), std::ios::in);

    if (!configFileStream)
    {
        std::cerr << "No config file found.";
        return 0;
    }

    nlohmann::json configJson;
    configFileStream >> configJson;

    const auto addr = readFromConfig<std::string>(configJson, ConfigFields::RemoteAddress);
    const auto port = readFromConfig<unsigned>(configJson, ConfigFields::RemotePort);

    if (!addr || !port)
    {
        std::cerr << "Cannot connect to the remote machine because not all parameters were provided in config.";
        return 0;
    }

    RemoteGamepad::Client client(*addr, *port);

    while (true)
    {
        client.syncWithRemote();

        std::this_thread::sleep_for(FrameDuration);
    }
}