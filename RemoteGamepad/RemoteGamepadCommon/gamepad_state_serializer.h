#pragma once

#include <string>
#include <sstream>
#include <string_view>

#include <Windows.h>
#include <Xinput.h>

#include <ViGEm/Client.h>


namespace RemoteGamepad
{
    static std::string serializeGamepadState(const XINPUT_STATE& state)
    {
        static std::stringstream stream;
        stream.str({});

        const auto& gamepadState = state.Gamepad;

        // !TODO: implement consistent serialization.
        stream.write(reinterpret_cast<const char*>(&gamepadState), sizeof(gamepadState));

        return stream.str();
    }

    static XUSB_REPORT deserializeGamepadState(const std::string& data)
    {
        XUSB_REPORT result;
        XUSB_REPORT_INIT(&result);

        static std::stringstream stream;
        stream.str(data);

        XINPUT_GAMEPAD gamepadState;
        stream.read(reinterpret_cast<char*>(&gamepadState), sizeof(gamepadState));
        result.bLeftTrigger = gamepadState.bLeftTrigger;
        result.bRightTrigger = gamepadState.bRightTrigger;
        result.sThumbLX = gamepadState.sThumbLX;
        result.sThumbLY = gamepadState.sThumbLY;
        result.sThumbRX = gamepadState.sThumbRX;
        result.sThumbRY = gamepadState.sThumbRY;
        result.wButtons = gamepadState.wButtons;

        return result;
    }
}
