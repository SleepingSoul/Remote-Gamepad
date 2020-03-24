#include <utils.h>
#include <Exception.h>
#include <logging.h>
#include "RemoteGamepadServer.h"


int main()
{
    std::cout << "RemoteGamepadServer (c) by Tihran Katolikian\n";

    RemoteGamepad::Logging::setUpDefaultGlobalLoggers();

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

        //!TODO: add normal exit condition to be sure all destructors will be called.
        //!TODO: add exit synchronization.
        while (true)
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