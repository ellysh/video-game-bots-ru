void WriteWord(HANDLE hProc, DWORD_PTR address, WORD value)
{
    if (WriteProcessMemory(hProc, (void*)address, &value, sizeof(value), NULL) == 0)
        printf("Failed to write memory: %u\n", GetLastError());
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
            WriteWord(hTargetProc, hpAddress, 100);

        Sleep(2000);
    }
    return 0;
}
