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
        struct GamepadUserConfiguration
        {
            float leftThumbDeadzone{ XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE };
            float rightThumbDeadzone{ XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE };
        };

        Client(std::string remoteMachineAddress, unsigned short remotePort, GamepadUserConfiguration userConfiguration);

        void connectToServer();
        void syncWithRemote();
        void closeConnection();

    private:
        bool isGamepadStateZero(XINPUT_GAMEPAD state) const;

        boost::asio::io_service m_IOService;
        boost::asio::ip::tcp::socket m_socket;

        std::string m_IPAddress;
        unsigned short m_port;

        GamepadUserConfiguration m_userConfiguration;

        XINPUT_GAMEPAD m_lastGamepadState;
    };
}
