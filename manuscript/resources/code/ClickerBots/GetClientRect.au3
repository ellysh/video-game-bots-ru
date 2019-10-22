#include <WinAPI.au3>

$hWnd = WinGetHandle("[CLASS:MSPaintApp]")
$tRECT = _WinAPI_GetClientRect($hWnd)
MsgBox(0, "Прямоугольник", _
       "Левый край: " & DllStructGetData($tRECT, "Left") & @CRLF & _
       "Правый край: " & DllStructGetData($tRECT, "Right") & @CRLF & _
       "Верхний край: " & DllStructGetData($tRECT, "Top") & @CRLF & _
       "Нижний край: " & DllStructGetData($tRECT, "Bottom"))
