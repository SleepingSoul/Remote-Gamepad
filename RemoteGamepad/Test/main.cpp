#include <cstdio>
#include <Windows.h>
#include <Xinput.h>

int main()
{
    std::puts("Prog start");
    XINPUT_STATE s;
    XInputGetState(0, &s);
    return 0;
}