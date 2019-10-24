WORD ReadWord(HANDLE hProc, DWORD_PTR address)
{
    WORD result = 0;

    if (ReadProcessMemory(hProc, (void*)address, &result, sizeof(result), NULL) == 0)
        printf("Failed to read memory: %u\n", GetLastError());

    return result;
}

int main()
{
    // Предоставить SE_DEBUG_NAME привилегию текущему процессу

    // Подключиться к процессу Diablo 2

    // Искать объект игрового персонажа в памяти процесса Diablo 2

    // Вычислить абсолютный адрес переменной с уровнем здоровья персонажа

    ULONG hp = 0;

    while (1)
    {
        hp = ReadWord(hTargetProc, hpAddress);
        printf("HP = %lu\n", hp);

        if (hp < 100)
            PostMessage(wnd, WM_KEYDOWN, 0x31, 0x1);

        Sleep(2000);
    }
    return 0;
}
