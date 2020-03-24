#include <utils.h>
#include <Exception.h>
#include <logging.h>
#include <StateManager.h>
#include "RemoteGamepadServer.h"


int main()
{
    std::cout << "RemoteGamepadServer (c) by Tihran Katolikian\n";

    RemoteGamepad::Logging::setUpDefaultGlobalLoggers();

    RemoteGamepad::StateManager::instance().setUp();

    RemoteGamepad::Logging::StdOut()->info("Reading config file...");

    const auto config = RemoteGamepad::readConfigFile();

    if (!config)
    {
        RemoteGamepad::Logging::StdErr()->critical("No config file found.");
        return 0;
    }

    const auto port = RemoteGamepad::readFromConfig<unsigned short>(*config, RemoteGamepad::ConfigFields::RemotePort);

    if (!port)
    {
        RemoteGamepad::Logging::StdErr()->critical("Cannot connect to the remote machine because not all parameters were provided in config.");
        return 0;
    }

    try
    {
        RemoteGamepad::Server server(*port);

        server.connectWithClient();
        
        while (!RemoteGamepad::StateManager::instance().shouldCloseProgram() && server.hasConnection())
        {
            server.receiveData();
        }
    }
    catch (const RemoteGamepad::Exception& error)
    {
        RemoteGamepad::Logging::StdErr()->critical(error);
    }

    std::cout << "Press any key to leave.";
    std::getchar();

    return 0;
}