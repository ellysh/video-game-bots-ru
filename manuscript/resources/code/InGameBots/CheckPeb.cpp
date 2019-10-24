#include <stdio.h>

int main()
{
    SHORT result = 0;

    while (gLife > 0)
    {
        int res = 0;
        __asm
        {
            mov eax, dword ptr fs:[18h]
            mov eax, dword ptr ds:[eax+30h]
            movzx eax, byte ptr ds:[eax+2h]
            mov res, eax
        };
        if (res)
        {
            printf("debugger detected!\n");
            exit(EXIT_FAILURE);
        }
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
