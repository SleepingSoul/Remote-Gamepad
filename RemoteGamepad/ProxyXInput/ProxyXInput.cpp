#include <Windows.h>
#include <Xinput.h>
#include <fstream>
#include <mutex>
#include <atomic>

#define PROXY_XINPUT_CALL_CONV WINAPI


namespace
{
    std::once_flag RealDLLLoadFlag;

    using RealDLLGetStateType = DWORD(WINAPI*)(DWORD, XINPUT_STATE*);

    std::atomic<RealDLLGetStateType> RealXInputGetState;

    void loadRealDLL()
    {
        HMODULE realXInputDLL = LoadLibraryExW(L"C:\\Users\\katol\\Remote-Gamepad\\RemoteGamepad\\ProxyXInput\\real_dll\\Real_XInput9_1_0.dll", nullptr, 0);

        const char* const realXInputGetStateName = "real_dll/XInputGetState";

        RealXInputGetState.store(reinterpret_cast<RealDLLGetStateType>(GetProcAddress(realXInputDLL, realXInputGetStateName)));
    }
}

extern "C"
{
    DWORD PROXY_XINPUT_CALL_CONV ProxyXInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState)
    {
        std::call_once(RealDLLLoadFlag, loadRealDLL);

        RealDLLGetStateType xinputGetState = RealXInputGetState.load();
        DWORD result = xinputGetState(dwUserIndex, pState);
        pState->Gamepad.wButtons = pState->Gamepad.wButtons | XINPUT_GAMEPAD_LEFT_THUMB;
        pState->Gamepad.wButtons = pState->Gamepad.wButtons | XINPUT_GAMEPAD_LEFT_SHOULDER;
        return result;
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        std::puts("DLL proc attach");
        break;
    }
    case DLL_PROCESS_DETACH:
    {
        std::puts("DLL proc detach");
        break;
    }
    case DLL_THREAD_ATTACH:
    {
        std::puts("DLL thread attach");
        break;
    }
    case DLL_THREAD_DETACH:
    {
        std::puts("DLL thread detach");
        break;
    }
    }

    return TRUE;
}
