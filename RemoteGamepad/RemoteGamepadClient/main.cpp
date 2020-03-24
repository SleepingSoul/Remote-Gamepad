#include <utils.h>
#include <Exception.h>
#include <logging.h>
#include <StateManager.h>
#include "RemoteGamepadClient.h"


int main()
{
    std::cout << "RemoteGamepadClient (c) by Tihran Katolikian\n";

    RemoteGamepad::Logging::setUpDefaultGlobalLoggers();

    RemoteGamepad::StateManager::instance().setUp();

    RemoteGamepad::Logging::StdOut()->info("Reading config file...");

    const auto config = RemoteGamepad::readConfigFile();

    if (!config)
    {
        RemoteGamepad::Logging::StdErr()->critical("No config file found.");
        return 0;
    }

    const auto addr = RemoteGamepad::readFromConfig<std::string>(*config, RemoteGamepad::ConfigFields::RemoteAddress);
    const auto port = RemoteGamepad::readFromConfig<unsigned short>(*config, RemoteGamepad::ConfigFields::RemotePort);
    
    RemoteGamepad::Client::GamepadUserConfiguration userConfiguration;

    if (const auto leftThumbDeadzone = RemoteGamepad::readFromConfig<float>(*config, RemoteGamepad::ConfigFields::LeftThumbDeadzone); leftThumbDeadzone.has_value())
    {
        userConfiguration.leftThumbDeadzone = *leftThumbDeadzone;
    }
    else
    {
        RemoteGamepad::Logging::StdOut()->warn("No '{}' field provided in config. Default value will be used: {}"
            , RemoteGamepad::ConfigFields::LeftThumbDeadzone, userConfiguration.leftThumbDeadzone);
    }

    if (const auto rightThumbDeadzone = RemoteGamepad::readFromConfig<float>(*config, RemoteGamepad::ConfigFields::RightThumbDeadzone); rightThumbDeadzone.has_value())
    {
        userConfiguration.rightThumbDeadzone = *rightThumbDeadzone;
    }
    else
    {
        RemoteGamepad::Logging::StdOut()->warn("No '{}' field provided in config. Default value will be used: {}"
            , RemoteGamepad::ConfigFields::RightThumbDeadzone, userConfiguration.rightThumbDeadzone);
    }

    if (!addr || !port)
    {
        RemoteGamepad::Logging::StdErr()->critical("Cannot connect to the remote machine because not all parameters were provided in config.");
        return 0;
    }

    try
    {
        RemoteGamepad::Client client(*addr, *port, userConfiguration);

        client.connectToServer();

        while (!RemoteGamepad::StateManager::instance().shouldCloseProgram())
        {
            client.syncWithRemote();

            RemoteGamepad::waitNextFrame();
        }

        client.closeConnection();
    }
    catch (const RemoteGamepad::Exception& error)
    {
        RemoteGamepad::Logging::StdErr()->critical(error);
    }

    std::cout << "Press any key to leave.";
    std::getchar();

    return 0;
}