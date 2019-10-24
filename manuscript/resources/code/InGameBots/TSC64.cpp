int main()
{
    SHORT result = 0;

    DWORD64 prevCounter = __rdtsc();

    while (gLife > 0)
    {
        DWORD64 counter = __rdtsc();

        if (MAX_DELTA < (counter - prevCounter))
        {
            printf("debugger detected!\n");
            exit(EXIT_FAILURE);
        }
        prevCounter = __rdtsc();

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
