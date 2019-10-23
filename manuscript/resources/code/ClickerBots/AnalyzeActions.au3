global const $gActionTemplate[3] = ['a', 'b', 'c']
global $gActionIndex = 0
global $gCounter = 0

func Reset()
    $gActionIndex = 0
    $gCounter = 0
endfunc

func AnalyzeKey($key)
    LogWrite("AnalyzeKey() - key = " & $key);

    $indexMax = UBound($gActionTemplate) - 1
    if $gActionIndex <= $indexMax and $key <> $gActionTemplate[$gActionIndex] then
        Reset()
        return
    endif

    if $gActionIndex < $indexMax and $key = $gActionTemplate[$gActionIndex] then
        $gActionIndex += 1
        return
    endif

    if $gActionIndex = $indexMax and $key = $gActionTemplate[$gActionIndex] then
        $gCounter += 1
        $gActionIndex = 0

        if $gCounter = 3 then
            MsgBox(0, "Alert", "Обнаружен бот-кликер!")
            Reset()
        endif
    endif
endfunc
