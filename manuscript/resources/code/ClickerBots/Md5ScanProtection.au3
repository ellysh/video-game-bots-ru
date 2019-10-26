#include <Crypt.au3>

global const $kLogFile = "debug.log"
global const $kCheckMd5[2] = ["0x3E4539E7A04472610D68B32D31BF714B", _
                              "0xD960F13A44D3BD8F262DF625F5705A63"]

func LogWrite($data)
    FileWrite($kLogFile, $data & chr(10))
endfunc

func _ProcessGetLocation($pid)
    local $proc = DllCall('kernel32.dll', 'hwnd', 'OpenProcess', 'int', _
                          BitOR(0x0400, 0x0010), 'int', 0, 'int', $pid)

    if $proc[0] = 0 then
        return ""
    endif

    local $struct = DllStructCreate('int[1024]')
    DllCall('psapi.dll', 'int', 'EnumProcessModules', 'hwnd', $proc[0], _
            'ptr', DllStructGetPtr($struct), 'int', _
            DllStructGetSize($struct), 'int_ptr', 0)

    local $return = DllCall('psapi.dll', 'int', 'GetModuleFileNameEx', _
        'hwnd', $proc[0], 'int', DllStructGetData($struct, 1), _
        'str', '', 'int', 2048)

    if StringLen($return[3]) = 0 then
        return ""
    endif

    return $return[3]
endfunc

func ScanProcess()
    local $processList = ProcessList()

    for $i = 1 to $processList[0][0]
        local $path = _ProcessGetLocation($processList[$i][1])
        local $md5 = _Crypt_HashFile($path, $CALG_MD5)
        LogWrite("Name: " & $processList[$i][0] & " PID: " & _
                 $processList[$i][1] & " Path: " & $path & " md5: " & $md5)

        for $j = 0 to Ubound($kCheckMd5) - 1
            if $md5 == $kCheckMd5[$j] then
                MsgBox(0, "Alert", "Обнаружен бот-кликер!")
            endif
        next
    next
endfunc

while true
    ScanProcess()
    Sleep(5000)
wend
