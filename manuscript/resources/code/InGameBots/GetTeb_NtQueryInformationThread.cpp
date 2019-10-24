#include <windows.h>
#include <winternl.h>

#pragma comment(lib,"ntdll.lib")

typedef struct _CLIENT_ID {
    DWORD UniqueProcess;
    DWORD UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _THREAD_BASIC_INFORMATION {
    typedef PVOID KPRIORITY;
    NTSTATUS ExitStatus;
    PVOID TebBaseAddress;
    CLIENT_ID ClientId;
    KAFFINITY AffinityMask;
    KPRIORITY Priority;
    KPRIORITY BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

typedef enum _THREADINFOCLASS2 {
    ThreadBasicInformation,
    ThreadTimes,
    ThreadPriority,
    ThreadBasePriority,
    ThreadAffinityMask,
    ThreadImpersonationToken,
    ThreadDescriptorTableEntry,
    ThreadEnableAlignmentFaultFixup,
    ThreadEventPair_Reusable,
    ThreadQuerySetWin32StartAddress,
    ThreadZeroTlsCell,
    ThreadPerformanceCount,
    ThreadAmILastThread,
    ThreadIdealProcessor,
    ThreadPriorityBoost,
    ThreadSetTlsArrayAddress,
    _ThreadIsIoPending,
    ThreadHideFromDebugger,
    ThreadBreakOnTermination,
    MaxThreadInfoClass
} THREADINFOCLASS2;

PTEB GetTeb()
{
    THREAD_BASIC_INFORMATION threadInfo;
    if (NtQueryInformationThread(GetCurrentThread(),
                                 (THREADINFOCLASS)ThreadBasicInformation,
                                 &threadInfo, sizeof(threadInfo), NULL))
    {
        printf("NtQueryInformationThread return error\n");
        return NULL;
    }
    return reinterpret_cast<PTEB>(threadInfo.TebBaseAddress);
}
