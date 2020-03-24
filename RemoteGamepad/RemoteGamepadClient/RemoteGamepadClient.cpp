#include "RemoteGamepadClient.h"
#include <iostream>
#include <optional>

#include <gamepad_state_serializer.h>
#include <logging.h>
#include <Exception.h>


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

    bool operator ==(const XINPUT_GAMEPAD& first, const XINPUT_GAMEPAD& second)
    {
        return first.bLeftTrigger == second.bLeftTrigger
            && first.bRightTrigger == second.bRightTrigger
            && first.sThumbLX == second.sThumbLX
            && first.sThumbLY == second.sThumbLY
            && first.sThumbRX == second.sThumbRX
            && first.sThumbRY == second.sThumbRY
            && first.wButtons == second.wButtons;
    }
}

RemoteGamepad::Client::Client(std::string remoteMachineAddress, unsigned short remotePort, GamepadUserConfiguration userConfiguration)
    : m_socket(m_IOService)
    , m_IPAddress(std::move(remoteMachineAddress))
    , m_port(remotePort)
    , m_userConfiguration(userConfiguration)
{
    ZeroMemory(&m_lastGamepadState, sizeof(m_lastGamepadState));
}

void RemoteGamepad::Client::connectToServer()
{
    using namespace boost::asio;

    ip::tcp::resolver::query resolverQuery(m_IPAddress, std::to_string(m_port), ip::tcp::resolver::query::numeric_service);

    ip::tcp::resolver resolver(m_IOService);

    boost::system::error_code error;

    ip::tcp::resolver::iterator it = resolver.resolve(resolverQuery, error);

    if (error.failed())
    {
        throw Exception("Connection to server failed because given IP address coudn't be resolved. Please, check if it is available.")
            .withArgument("error code", error.value())
            .withArgument("error message", error.message());
    }

    Logging::StdOut()->info("Attempting to connect to the server with IP: {}", it->endpoint().address().to_string());
    m_socket.connect(it->endpoint(), error);

    if (error.failed())
    {
        throw Exception("Connection unsuccsessful.")
            .withArgument("error code", error.value())
            .withArgument("error message", error.message());
    }

    Logging::StdOut()->info("Succesfully connected to server.");

    // Set keep alive option to always be sure out impul will reach the server, even if gamepad is idling for a long time.
    const boost::asio::socket_base::keep_alive keepAliveOption(true);
    m_socket.set_option(keepAliveOption, error);

    if (error.failed())
    {
        Logging::StdErr()->error("Couldn't set 'keep alive' option. The connection may be interrupted in future. Error code: {}, error message: {}"
            , error.value(), error.message());
    }
    else
    {
        Logging::StdOut()->info("Succesfully set 'keep alive' option.");
    }
}

void RemoteGamepad::Client::syncWithRemote()
{
    const auto localState = captureLocalGamepadState();

    if (!localState)
    {
        Logging::StdErr()->warn("Couldn't capture local gamepad state.");
        return;
    }

    const bool currentGamepadStateIsZero = isGamepadStateZero(localState->Gamepad);
    const bool prevGamepadStateIsZero = isGamepadStateZero(m_lastGamepadState);

    if (currentGamepadStateIsZero)
    {
        if (prevGamepadStateIsZero)
        {
            return;
        }
        else
        {
            Logging::StdOut()->debug("Gamepad goes into idling state, no updates will be sent.");
        }
    }

    m_lastGamepadState = localState->Gamepad;

    const auto dataToSend = RemoteGamepad::serializeGamepadState(*localState);

    boost::system::error_code error;

    boost::asio::write(m_socket, boost::asio::buffer(dataToSend), error);

    if (error.failed())
    {
       Logging::StdErr()->error("Coudn't write a message into a socket. Error code: {}, message: {}", error.value(), error.message());
    }
}

void RemoteGamepad::Client::closeConnection()
{
    boost::system::error_code error;

    Logging::StdOut()->info("Attempting to close the active connection...");
    m_socket.close(error);

    if (error.failed())
    {
        throw Exception("Error occured when connection was being closed.")
            .withArgument("error code", error.value())
            .withArgument("error message", error.message());
    }

    Logging::StdOut()->info("Active connection succesfully closed.");
}

bool RemoteGamepad::Client::isGamepadStateZero(XINPUT_GAMEPAD state) const
{
    static const XINPUT_GAMEPAD zeroInputGamepadState = []
    {
        XINPUT_GAMEPAD state;

        ZeroMemory(&state, sizeof(state));

        return state;
    }();

    const auto isStickZero = [](float x, float y, float deadZone)
    {
        const float magnitude2 = x * x + y * y;

        return magnitude2 < deadZone * deadZone;
    };

    if (isStickZero(state.sThumbLX, state.sThumbLY, m_userConfiguration.leftThumbDeadzone))
    {
        state.sThumbLX = state.sThumbLY = 0;
    }

    if (isStickZero(state.sThumbRX, state.sThumbRY, m_userConfiguration.rightThumbDeadzone))
    {
        state.sThumbRX = state.sThumbRY = 0;
    }

    return state == zeroInputGamepadState;
}