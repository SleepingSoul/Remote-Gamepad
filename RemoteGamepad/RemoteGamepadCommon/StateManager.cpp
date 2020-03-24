#include "StateManager.h"


namespace
{
    void ctrlCEventImpl()
    {
        RemoteGamepad::Logging::StdOut()->info("Keyboard interrupt by user.");
        RemoteGamepad::StateManager::instance().setShouldCloseProgram();
    }
}

extern "C"
{
    BOOL WINAPI ctrlHandler(DWORD ctrlType)
    {
        switch (ctrlType)
        {
        case CTRL_C_EVENT:
            ctrlCEventImpl();
            return TRUE;
        default:
            return FALSE;
        }
    }
}
