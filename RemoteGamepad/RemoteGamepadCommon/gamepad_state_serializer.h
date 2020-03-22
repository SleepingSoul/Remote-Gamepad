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

#ifdef _DEBUG
        // Write data in human-readable format.
        stream << gamepadState.bLeftTrigger << gamepadState.bRightTrigger << gamepadState.sThumbLX << gamepadState.sThumbLY
            << gamepadState.sThumbRX << gamepadState.sThumbRY << gamepadState.wButtons << '\n';
#else
#error Not implemented
#endif

        return stream.str();
    }

    static XUSB_REPORT deserializeGamepadState(const std::string& data)
    {
        XUSB_REPORT result;
        XUSB_REPORT_INIT(&result);

        static std::stringstream ss;
        ss.str(data);

#ifdef _DEBUG
        ss >> result.bLeftTrigger >> result.bRightTrigger >> result.sThumbLX >> result.sThumbLY
            >> result.sThumbRX >> result.sThumbRY >> result.wButtons;
#else
#error Not implemented
#endif

        return result;
    }
}
