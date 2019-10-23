func Move()
    SRandom(@MSEC)
    MouseClick("left", Random(300, 800), Random(170, 550), 1)
endfunc

func Attack()
    LogWrite("Attack()")

    const $TimeoutMax = 10
    $timeout = 0
    while IsTargetExist() and $timeout < $TimeoutMax
        Send("{F1}")
        Sleep(2000)

        Send("{F2}")
        Sleep(2000)

        $timeout += 1
    wend

    if $timeout == $TimeoutMax then
        Move()
    endif
endfunc
