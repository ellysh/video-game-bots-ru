global const $gKeyHandler = "_KeyHandler"
global const $kLogFile = "debug.log"

global $gTimeSpanA = -1
global $gPrevTimestampA = -1

func LogWrite($data)
    FileWrite($kLogFile, $data & chr(10))
endfunc

func _KeyHandler()
    ; См листинг 2-29
endfunc

func InitKeyHooks($handler)
    for $i = 0 to 256
        HotKeySet(Chr($i), $handler)
    next
endfunc

func AnalyzeKey($key)
    ; См листинг 2-30
endfunc

InitKeyHooks($gKeyHandler)

while true
    Sleep(10)
wend
