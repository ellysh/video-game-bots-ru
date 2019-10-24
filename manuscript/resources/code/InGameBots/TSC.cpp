#include <stdio.h>
#include <stdint.h>
#include <windows.h>

static const DWORD64 MAX_DELTA = 2650000000;

static const uint16_t MAX_LIFE = 20;
static uint16_t gLife = MAX_LIFE;

#define ReadRdtsc(result) \
{ \
__asm cpuid \
__asm rdtsc \
__asm mov dword ptr[result + 0], eax \
__asm mov dword ptr[result + 4], edx \
}

int main()
{
    SHORT result = 0;

    DWORD64 prevCounter = 0;
    ReadRdtsc(prevCounter);

    while (gLife > 0)
    {
        DWORD64 counter = 0;
        ReadRdtsc(counter);

        if (MAX_DELTA < (counter - prevCounter))
        {
            printf("debugger detected!\n");
            exit(EXIT_FAILURE);
        }
        ReadRdtsc(prevCounter);

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
