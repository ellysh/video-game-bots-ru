SIZE_T IsArrayMatch(HANDLE proc, SIZE_T address, SIZE_T segmentSize, BYTE array[], SIZE_T arraySize)
{
    BYTE* procArray = new BYTE[segmentSize];

    if (ReadProcessMemory(proc, (void*)address, procArray, segmentSize, NULL) != 0)
    {
        printf("Failed to read memory: %u\n", GetLastError());
        delete[] procArray;
        return 0;
    }

    for (SIZE_T i = 0; i < segmentSize; ++i)
    {
        if ((array[0] == procArray[i]) && ((i + arraySize) < segmentSize))
        {
            if (!memcmp(array, procArray + i, arraySize))
            {
                delete[] procArray;
                return address + i;
            }
        }
    }

    delete[] procArray;
    return 0;
}

SIZE_T ScanSegments(HANDLE proc, BYTE array[], SIZE_T size)
{
    MEMORY_BASIC_INFORMATION meminfo;
    LPCVOID addr = 0;
    SIZE_T result = 0;

    if (!proc)
        return 0;

    while (1)
    {
        if (VirtualQueryEx(proc, addr, &meminfo, sizeof(meminfo)) == 0)
            break;

        if ((meminfo.State & MEM_COMMIT) && (meminfo.Type & MEM_PRIVATE) && (meminfo.Protect & PAGE_READWRITE) && !(meminfo.Protect & PAGE_GUARD))
        {
            result = IsArrayMatch(proc, (SIZE_T)meminfo.BaseAddress, 
                    meminfo.RegionSize, array, size);

            if (result != 0)
                return result;
        }
        addr = (unsigned char*)meminfo.BaseAddress + meminfo.RegionSize;
    }
    return 0;
}

int main()
{
    // Предоставить SE_DEBUG_NAME привилегию текущему процессу

    // Подключиться к процессу Diablo 2

    BYTE array[] = { 0, 0, 0, 0, 0x04, 0, 0, 0, 0x03, 0, 0x28, 0x0F, 0, 0x4B, 0x61, 0x69, 0x6E, 0, 0, 0 };

    SIZE_T objectAddress = ScanSegments(hTargetProc, array, sizeof(array));

    return 0;
}
