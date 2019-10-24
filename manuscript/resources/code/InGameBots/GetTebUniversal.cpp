#include <windows.h>
#include <winternl.h>

PTEB GetTeb()
{
#if defined(_M_X64) // x64
    PTEB pTeb = reinterpret_cast<PTEB>(__readgsqword(reinterpret_cast<DWORD>(
                                       &static_cast<PNT_TIB>(nullptr)->Self)));
#else // x86
    PTEB pTeb = reinterpret_cast<PTEB>(__readfsdword(reinterpret_cast<DWORD>(
                                       &static_cast<PNT_TIB>(nullptr)->Self)));
#endif
    return pTeb;
}
