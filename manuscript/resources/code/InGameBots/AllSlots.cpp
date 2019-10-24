    ULONG hp = 0;
    BYTE keys[] = { 0x31, 0x32, 0x33, 0x34 };
    BYTE keyIndex = 0;

    while (1)
    {
        hp = ReadWord(hTargetProc, hpAddress);
        printf("HP = %lu\n", hp);

        if (hp < 100)
        {
            PostMessage(wnd, WM_KEYDOWN, keys[keyIndex], 0x1);
            ++keyIndex;
            if (keyIndex == sizeof(keys))
                keyIndex = 0;
        }
        Sleep(2000);
    }
