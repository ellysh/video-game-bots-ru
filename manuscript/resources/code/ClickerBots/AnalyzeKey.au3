global $gTimeSpanA = -1
global $gPrevTimestampA = -1

func AnalyzeKey($key)
    local $timestamp = (@SEC * 1000 + @MSEC)
    LogWrite("AnalyzeKey() - key = " & $key & " msec = " & $timestamp)
    if $key <> 'a' then
        return
    endif

    if $gPrevTimestampA = -1 then
        $gPrevTimestampA = $timestamp
        return
    endif

    local $newTimeSpan = $timestamp - $gPrevTimestampA
    $gPrevTimestampA = $timestamp

    if $gTimeSpanA = -1 then
        $gTimeSpanA = $newTimeSpan
        return
    endif

    if Abs($gTimeSpanA - $newTimeSpan) < 100 then
        MsgBox(0, "Alert", "Обнаружен бот-кликер!")
    endif
endfunc
