#pragma once

#include <string_view>

#include <boost/asio.hpp>

#include <Windows.h>
#include <XInput.h>


namespace RemoteGamepad
{
    class Client
    {
    public:
        Client(std::string remoteMachineAddress, unsigned short remotePort);

        void connectToServer();

        void syncWithRemote();

    private:
        boost::asio::io_service m_IOService;
        boost::asio::ip::tcp::socket m_socket;

        std::string m_IPAddress;
        unsigned short m_port;

        XINPUT_GAMEPAD m_lastGamepadState;
    };
}
