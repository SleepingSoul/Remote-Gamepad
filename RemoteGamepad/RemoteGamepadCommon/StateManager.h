#pragma once

#include <Windows.h>

#include "logging.h"


extern "C"
{
    BOOL WINAPI ctrlHandler(DWORD ctrlType);
}

namespace RemoteGamepad
{
    class StateManager
    {
    public:
        static StateManager& instance()
        {
            static StateManager manager{};
            return manager;
        }

        void setUp()
        {
            if (!SetConsoleCtrlHandler(ctrlHandler, TRUE))
            {
                Logging::StdErr()->error("Control handler was not set. You may not be able to leave the program in a correct way.");
            }
        }

        void setShouldCloseProgram() { m_shouldCloseProgram = true; }
        bool shouldCloseProgram() const { return m_shouldCloseProgram; }

    private:
        bool m_shouldCloseProgram{ false };
    };
}
