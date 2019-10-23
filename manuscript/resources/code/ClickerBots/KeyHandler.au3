global const $gKeyHandler = "_KeyHandler"

func _KeyHandler()
    $keyPressed = @HotKeyPressed

    LogWrite("_KeyHandler() - asc = " & asc($keyPressed) & " key = " & $keyPressed)
    AnalyzeKey($keyPressed)

    HotKeySet($keyPressed)
    Send($keyPressed)
    HotKeySet($keyPressed, $gKeyHandler)
endfunc

func InitKeyHooks($handler)
    for $i = 0 to 255
        HotKeySet(Chr($i), $handler)
    next
endfunc

InitKeyHooks($gKeyHandler)

while true
    Sleep(10)
wend
