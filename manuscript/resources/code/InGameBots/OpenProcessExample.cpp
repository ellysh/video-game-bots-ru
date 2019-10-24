int main()
{
    Sleep(4000);

    HWND wnd = GetForegroundWindow();
    DWORD pid = 0;
    if (!GetWindowThreadProcessId(wnd, &pid))
    {
        printf("Error of the pid detection\n");
        return 1;
    }

    HANDLE hTargetProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hTargetProc)
    {
        printf("Failed to open process: %u\n", GetLastError());
    }
    return 0;
}
