#include <stdio.h>
#include <windows.h>

BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
    // Смотрите реализацию этой функции в листинге 3-1
}

DWORD ReadDword(HANDLE hProc, DWORD_PTR address)
{
    DWORD result = 0;

    if (ReadProcessMemory(hProc, (void*)address, &result, sizeof(result), NULL) == 0)
    {
        printf("Failed to read memory: %u\n", GetLastError());
    }
    return result;
}

void WriteDword(HANDLE hProc, DWORD_PTR address, DWORD value)
{
    if (WriteProcessMemory(hProc, (void*)address, &value, sizeof(value), NULL) == 0)
    {
        printf("Failed to write memory: %u\n", GetLastError());
    }
}

int main()
{
    HANDLE hProc = GetCurrentProcess();

    HANDLE hToken = NULL;
    if (!OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES, &hToken))
        printf("Failed to open access token\n");

    if (!SetPrivilege(hToken, SE_DEBUG_NAME, TRUE))
        printf("Failed to set debug privilege\n");

    DWORD pid = 5356;
    HANDLE hTargetProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hTargetProc)
        printf("Failed to open process: %u\n", GetLastError());

    DWORD_PTR address = 0x001E0000;
    WriteDword(hTargetProc, address, 0xDEADBEEF);
    printf("Result of reading dword at 0x%llx address = 0x%x\n", address,
           ReadDword(hTargetProc, address));

    CloseHandle(hTargetProc);
    return 0;
}
