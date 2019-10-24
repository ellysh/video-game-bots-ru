#include <stdio.h>
#include <stdint.h>
#include <windows.h>

static const DWORD MAX_DELTA = 1020;

static const uint16_t MAX_LIFE = 20;
static uint16_t gLife = MAX_LIFE;

int main()
{
    SHORT result = 0;

    DWORD prevCounter = GetTickCount();

    while (gLife > 0)
    {
        if (MAX_DELTA < (GetTickCount() - prevCounter))
        {
            printf("debugger detected!\n");
            exit(EXIT_FAILURE);
        }
        prevCounter = GetTickCount();

        result = GetAsyncKeyState(0x31);
        if (result != 0xFFFF8001)
            --gLife;
        else
            ++gLife;

        printf("life = %u\n", gLife);
        Sleep(1000);
    }

    printf("stop\n");

    return 0;
}
