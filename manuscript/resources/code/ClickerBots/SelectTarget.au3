func SelectTarget()
    LogWrite("SelectTarget()")

    while not IsTargetExist()
        Send("{F10}")
        Sleep(200)

        if IsTargetExist() then
            exitloop
        endif

        Send("{F9}")
        Sleep(200)
    wend
endfunc
