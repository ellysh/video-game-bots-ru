#RequireAdmin

func SelectTarget()
    Send("{F9}")
    Sleep(200)
endfunc

func Attack()
    Send("{F1}")
    Sleep(5000)
endfunc

func Pickup()
    Send("{F8}")
    Sleep(1000)
endfunc

Sleep(2000)

while true
    SelectTarget()
    Attack()
    Pickup()
wend
