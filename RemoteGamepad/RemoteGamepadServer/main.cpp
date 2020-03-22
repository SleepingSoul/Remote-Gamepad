#include <utils.h>
#include "RemoteGamepadServer.h"

int main()
{
    std::cout << "RemoteGamepadServer (c) by Tihran Katolikian\n" << "\nReading config file...\n";

    const auto config = RemoteGamepad::readConfigFile();

    if (!config)
    {
        std::cerr << "No config file found.\n";
        return 0;
    }

    const auto port = RemoteGamepad::readFromConfig<unsigned short>(*config, RemoteGamepad::ConfigFields::RemotePort);

    if (!port)
    {
        std::cerr << "Cannot connect to the remote machine because not all parameters were provided in config.\n";
        return 0;
    }

    RemoteGamepad::Server server(*port);

    server.receive();

    return 0;
}