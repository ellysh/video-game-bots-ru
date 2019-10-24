#include <windows.h>
#include <winternl.h>

PTEB GetTeb()
{
#if defined(_M_X64) // x64
    PTEB pTeb = reinterpret_cast<PTEB>(__readgsqword(0x30));
#else // x86
    PTEB pTeb = reinterpret_cast<PTEB>(__readfsdword(0x18));
#endif
    return pTeb;
}
