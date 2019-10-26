VOID ProcessParam(__in Deviare2::INktParam *lpParam)
{
    CComBSTR cBstrName;
    lpParam->get_Name(&cBstrName);

    unsigned long val = 0;
    HRESULT hRes = lpParam->get_ULongVal(&val);
    if (FAILED(hRes))
        return;

    wprintf(L"ProcessParam() - name = %s value = %u\n",
            (BSTR)cBstrName, (unsigned int)(val));

    if (val < 10)
    {
        INPUT Input = { 0 };
        Input.type = INPUT_KEYBOARD;
        Input.ki.wVk = '1';
        SendInput( 1, &Input, sizeof( INPUT ) );
    }
}
