extern "C" BOOL __stdcall TextOutA_wrapper(
    _In_ HDC     hdc,
    _In_ int     nXStart,
    _In_ int     nYStart,
    _In_ LPCSTR lpString,
    _In_ int     cchString
    )
{
    if (cchString < 10)
    {
        INPUT Input = { 0 };
        Input.type = INPUT_KEYBOARD;
        Input.ki.wVk = '1';
        SendInput(1, &Input, sizeof(INPUT));
    }

    typedef BOOL(__stdcall *pS)(HDC, int, int, LPCTSTR, int);
    pS pps = (pS)mProcs[696];
    return pps(hdc, nXStart, nYStart, lpString, cchString);
}
