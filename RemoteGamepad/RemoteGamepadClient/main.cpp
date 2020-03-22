#include <utils.h>
#include "RemoteGamepadClient.h"

int main()
{
    std::cout << "RemoteGamepadClient (c) by Tihran Katolikian\n" << "\nReading config file...\n";

    const auto config = RemoteGamepad::readConfigFile();

    if (!config)
    {
        std::cerr << "No config file found.\n";
        return 0;
    }

    const auto addr = RemoteGamepad::readFromConfig<std::string>(*config, RemoteGamepad::ConfigFields::RemoteAddress);
    const auto port = RemoteGamepad::readFromConfig<unsigned short>(*config, RemoteGamepad::ConfigFields::RemotePort);

    if (!addr || !port)
    {
        std::cerr << "Cannot connect to the remote machine because not all parameters were provided in config.\n";
        return 0;
    }

    RemoteGamepad::Client client(*addr, *port);

    while (true)
    {
        client.syncWithRemote();

        RemoteGamepad::waitNextFrame();
    }

    return 0;
}