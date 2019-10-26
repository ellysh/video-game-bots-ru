HINSTANCE mHinst = 0, mHinstDLL = 0;
UINT_PTR mProcs[727] = {0};
LPCSTR mImportNames[] = {...}

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved ) {
    mHinst = hinstDLL;
    if ( fdwReason == DLL_PROCESS_ATTACH ) {
        mHinstDLL = LoadLibrary( "C:\\Windows\\SysWOW64\\gdi32.dll" );
        if ( !mHinstDLL )
            return ( FALSE );
        for (int i = 0; i < 727; i++)
        {
            mProcs[i] = (UINT_PTR)GetProcAddress(mHinstDLL, mImportNames[i]);
        }
    } else if ( fdwReason == DLL_PROCESS_DETACH ) {
        FreeLibrary( mHinstDLL );
    }
    return ( TRUE );
}
