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

    static bool operator ==(const XINPUT_GAMEPAD& first, const XINPUT_GAMEPAD& second)
    {
        return first.bLeftTrigger == second.bLeftTrigger
            && first.bRightTrigger == second.bRightTrigger
            && first.sThumbLX == second.sThumbLX
            && first.sThumbLY == second.sThumbLY
            && first.sThumbRX == second.sThumbRX
            && first.sThumbRY == second.sThumbRY
            && first.wButtons == second.wButtons;
    }

    static XINPUT_GAMEPAD makeZeroInputState()
    {
        XINPUT_GAMEPAD state;

        ZeroMemory(&state, sizeof(state));

        return state;
    }

    bool isGamepadStateZero(XINPUT_GAMEPAD state)
    {
        static const XINPUT_GAMEPAD zeroInputGamepadState = makeZeroInputState();

        const auto isStickZero = [](float x, float y, float deadZone)
        {
            const float magnitude2 = x * x + y * y;

            return magnitude2 < deadZone * deadZone;
        };

        if (isStickZero(state.sThumbLX, state.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
        {
            state.sThumbLX = state.sThumbLY = 0;
        }

        if (isStickZero(state.sThumbRX, state.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
        {
            state.sThumbRX = state.sThumbRY = 0;
        }

        return state == zeroInputGamepadState;
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
            << ", message: " << error.message() << '\n';
        throw error;
    }

    try
    {
        std::cout << "Attempting to connect to the server with IP: " << it->endpoint().address().to_string() << '\n';
        m_socket.connect(it->endpoint());
        std::cout << "Succesfully connected to server.\n";
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

#ifdef _DEBUG
    if (isGamepadStateZero(localState->Gamepad))
    {
        std::cout << "Current gamepad state is zero: not sending the data on server.\n";
        return;
    }
#endif

    const auto dataToSend = RemoteGamepad::serializeGamepadState(*localState);

    if (dataToSend.empty())
    {
        std::cout << "Gamepad state is zero: do not send any updates on server.\n";
        return;
    }

    boost::system::error_code error;

    boost::asio::write(m_socket, boost::asio::buffer(dataToSend), error);

    if (error.failed())
    {
        std::cerr << "Error: coudn't write a message into a socket. Error code: " << error.value() << ", message: " << error.message() << '\n';
        return;
    }
}
