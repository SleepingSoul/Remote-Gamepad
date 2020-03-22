#include "RemoteGamepadClient.h"
#include <iostream>
#include <optional>
#include <sstream>
#include <Windows.h>
#include <Xinput.h>

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

    std::string serializeGamepadState(const XINPUT_STATE& state)
    {
        static std::stringstream stream;
        stream.str({});

        const auto& gamepadState = state.Gamepad;

#ifdef _DEBUG
        // Write data in human-readable format.
        stream << gamepadState.bLeftTrigger << gamepadState.bRightTrigger << gamepadState.sThumbLX << gamepadState.sThumbLY
            << gamepadState.sThumbRX << gamepadState.sThumbRY << gamepadState.wButtons;
#else
#error Not implemented
#endif

        return stream.str();
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
        std::cerr << "Connection unsuccessful. Code: " << error.code() << ' ' << error.what();
        throw error;
    }
}

void RemoteGamepad::Client::syncWithRemote()
{
    const auto localState = captureLocalGamepadState();

    if (!localState)
    {
        std::cerr << "Sync failed: couldn't capture local gamepad state.";
        return;
    }

    const auto dataToSend = serializeGamepadState(*localState);

    boost::system::error_code error;

    boost::asio::write(m_socket, boost::asio::buffer(dataToSend), error);

    if (error.failed())
    {
        std::cerr << "Error: coudn't write a message into a socket. Error code: " << error.value() << ", message: " << error.message();
        return;
    }
}
