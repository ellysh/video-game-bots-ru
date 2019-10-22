#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

using namespace std;

typedef LPCTSTR(CALLBACK* LPFNDLLFUNC1)(void);

HINSTANCE hDLL;               // Дескриптор DLL-библиотеки
LPFNDLLFUNC1 lpfnDllFunc1;    // Указатель на функцию
LPCTSTR uReturnVal;

int main()
{
    hDLL = LoadLibraryA("FastFind");
    if (hDLL != NULL)
    {
        lpfnDllFunc1 = (LPFNDLLFUNC1)GetProcAddress(hDLL,
            "FFVersion");
        if (!lpfnDllFunc1)
        {
            // Обработка ошибки
            FreeLibrary(hDLL);
            cout << "error" << endl;
            return 1;
        }
        else
        {
            // Вызов функции из библиотеки
            uReturnVal = lpfnDllFunc1();
            cout << "version = " << uReturnVal << endl;
        }
    }
    return 0;
}
