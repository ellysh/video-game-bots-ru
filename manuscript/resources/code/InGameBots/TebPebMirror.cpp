#include <windows.h>
#include <winternl.h>

BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
    // Смотрите реализацию этой функции в листинге 3-1
}

BOOL GetMainThreadTeb(DWORD dwPid, PTEB pTeb)
{
    LPVOID tebAddress = NtCurrentTeb();
    printf("TEB = %p\n", tebAddress);

    HANDLE hProcess = OpenProcess(PROCESS_VM_READ, FALSE, dwPid);
    if (hProcess == NULL)
        return false;

    if (ReadProcessMemory(hProcess, tebAddress, pTeb, sizeof(TEB), NULL) == FALSE)
    {
        CloseHandle(hProcess);
        return false;
    }

    CloseHandle(hProcess);
    return true;
}

int main()
{
    HANDLE hProc = GetCurrentProcess();

    HANDLE hToken = NULL;
    if (!OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES, &hToken))
        printf("Failed to open access token\n");

    if (!SetPrivilege(hToken, SE_DEBUG_NAME, TRUE))
        printf("Failed to set debug privilege\n");

    DWORD pid = 7368;

    TEB teb;
    if (!GetMainThreadTeb(pid, &teb))
        printf("Failed to get TEB\n");

    printf("PEB = %p StackBase = %p\n", teb.ProcessEnvironmentBlock,
           teb.Reserved1[1]);

    return 0;
}
