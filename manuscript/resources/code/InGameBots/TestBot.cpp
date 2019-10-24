#include <stdio.h>
#include <windows.h>

BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
    // См. реализацию этой функции в листинге 3-1
}

SIZE_T ScanSegments(HANDLE proc)
{
    MEMORY_BASIC_INFORMATION meminfo;
    LPCVOID addr = 0;

    if (!proc)
        return 0;

    while (1)
    {
        if (VirtualQueryEx(proc, addr, &meminfo, sizeof(meminfo)) == 0)
            break;

        if ((meminfo.State == MEM_COMMIT) && (meminfo.Type & MEM_IMAGE) && (meminfo.Protect == PAGE_READWRITE) && (meminfo.RegionSize == 0x1000))
        {
            return (SIZE_T)meminfo.BaseAddress;
        }
        addr = (unsigned char*)meminfo.BaseAddress + meminfo.RegionSize;
    }
    return 0;
}

WORD ReadWord(HANDLE hProc, DWORD_PTR address)
{
    // См. реализацию этой функции в листинге 3-13
}

void WriteWord(HANDLE hProc, DWORD_PTR address, WORD value)
{
    if (WriteProcessMemory(hProc, (void*)address, &value, sizeof(value), NULL) == 0)
        printf("Failed to write memory: %u\n", GetLastError());
}

int main()
{
    // Предоставить SE_DEBUG_NAME привилегию текущему процессу

    // Подключиться к процессу тестового приложения

    SIZE_T lifeAddress = ScanSegments(hTargetProc);

    ULONG hp = 0;
    while (1)
    {
        hp = ReadWord(hTargetProc, lifeAddress);
        printf("life = %lu\n", hp);

        if (hp < 10)
            WriteWord(hTargetProc, lifeAddress, 20);

        Sleep(1000);
    }
    return 0;
}
