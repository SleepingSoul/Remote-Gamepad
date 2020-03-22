#include "RemoteGamepadClient.h"
#include <iostream>
#include <optional>

#include <Windows.h>
#include <XInput.h>

#include <gamepad_state_serializer.h>

namespace
{
    std::optional<XINPUT_STATE> captureLocalGamepadState()
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(state));

        // always use controller 0.
        // !TODO: multiple controllers?
        const DWORD controllerIndex = 0;

        const DWORD result = XInputGetState(controllerIndex, &state);

        if (result == ERROR_SUCCESS)
        {
            return state;
        }

        return std::nullopt;
    }
}

RemoteGamepad::Client::Client(const std::string& remoteMachineAddress, unsigned short remotePort)
    : m_socket(m_IOService)
{
    using namespace boost::asio;

    ip::tcp::resolver::query resolverQuery(remoteMachineAddress, std::to_string(remotePort), ip::tcp::resolver::query::numeric_service);

    ip::tcp::resolver resolver(m_IOService);

    boost::system::error_code error;

    ip::tcp::resolver::iterator it = resolver.resolve(resolverQuery, error);

    if (error.failed())
    {
        std::cerr << "Error: couldn't resolve DNS name: " << remoteMachineAddress << ", error code: " << error.value()
            << ", message: " << error.message();
        throw error;
    }

    try
    {
        m_socket.connect(it->endpoint());
    }
    catch (const boost::system::system_error& error)
    {
        std::cerr << "Connection unsuccessful. Code: " << error.code() << ' ' << error.what() << '\n';
        throw error;
    }
}

void RemoteGamepad::Client::syncWithRemote()
{
    const auto localState = captureLocalGamepadState();

    if (!localState)
    {
        std::cerr << "Sync failed: couldn't capture local gamepad state.\n";
        return;
    }

    const auto dataToSend = RemoteGamepad::serializeGamepadState(*localState);

    boost::system::error_code error;

    boost::asio::write(m_socket, boost::asio::buffer(dataToSend), error);

    if (error.failed())
    {
        std::cerr << "Error: coudn't write a message into a socket. Error code: " << error.value() << ", message: " << error.message() << '\n';
        return;
    }
}
