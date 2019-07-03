# Приёмы защиты от внутриигровых ботов

Мы реализовали внутриигрового бота для Diablo 2 и узнали, как он получает доступ к памяти процесса игры. Теперь рассмотрим способы защиты от этого типа ботов.

Есть две группы методов защиты от внутриигровых ботов:

* Защита приложения от реверс-инжиниринга.

* Блокировака алгоритмов бота.

Первая группа методов разрабатывается очень давно: со времён первых версий коммерческого ПО, которое нужно было защищать от нелицензионного распространения. Эти методы хорошо известны и документированны в Интернете. Их основаня задача - усложнить анализ приложения с помощью отладчика и дизассемблера.

Вторая группа методов защищает данные процесса игрового приложения от чтения и записи. Таким образом боту становится сложнее читать состояние объектов и внедрять свои действия.

Некоторые методы защиты могут относится сразу к обоим группам.

## Тестовое приложение

Вспомним архитетуру клиент-сервер современных онлайн игр. Клиент выполняется на компьютере пользователя и обменивается сообщениями с игровым сервером. Большая часть методов защиты от внутриигровых ботов работают на стороне клиента.

Чтобы познакомиться с методами защиты, напишем простое приложение, имитирующее состояние игрового объекта. Также разработаем простейшего внутриигрового бота, который подключается к нашему приложению и управляет им.

Алгоритм тестового приложения может быть следующим:

1. При старте присвоить параметру объекта (например уровень здоровья) максимально допустимое значение.

2. В цикле проверять состояние горячей клавиши "1".

3. Если пользователь не нажимает клавишу, уменьшать параметр объект. Иначе - увеличивать.

4. Если параметр объекта оказался равень 0, завершить приложение.

Листинг 3-16 демонстрирует исходный код тестового приложения.

**Листинг 3-16.** *Запись нового значения параметра персонажа в память процесса*
```C++
#include <stdio.h>
#include <stdint.h>
#include <windows.h>

static const uint16_t MAX_LIFE = 20;
static uint16_t gLife = MAX_LIFE;

int main()
{
    SHORT result = 0;

    while (gLife > 0)
    {
        result = GetAsyncKeyState(0x31);
        if (result != 0xFFFF8001)
            --gLife;
        else
            ++gLife;
 
        printf("life = %u\n", gLife);
        Sleep(1000);
    }
    printf("stop\n");
    return 0;
}
```
Уровень здоровья игрового объекта хранится в глобальной переменной `gLife`. При старте приложения мы присваиваем ей значение константы `MAX_LIFE` (равно 20).

В функции `main` есть цикл `while`, в котором мы проверяем состояние клавиши с помощью WinAPI функции `GetAsyncKeyState`. Код этой клавиши передаётся в функцию входным параметром. Виртуальный код клавиши "1" равен 0x31. Если вызов `GetAsyncKeyState` возвращает состояние "не нажато", переменная `gLife` уменьшается на 1. В противном случае - увеличивается также на 1. После этого идёт односекундная задержка, которая нужна чтобы пользователь успел отпустить клавишу.

Попробуйте скомпилировать тестовое приложение в конфигурации "Debug" (отладка) в Visual Studio и запустить его.

## Исследование памяти тестового приложения

Теперь напишем бота для нашего тестового приложения. Его алгоритм будет таким же, как и для игры Diablo 2 из прошлого раздела. Если параметр здоровья опускается ниже 10, бот симулирует нажатие клавиши "1".

Бот должен читать значение переменной `gLife`. Очевидно, мы не можем воспользоваться механизмом поиска объекта из бота для Diablo 2. Нам нужно провести полный анализ адресного пространства тестового приложения и разработать метод доступа к `gLife`. Хорошая новость в том, что это приложение очень простое и нам будет достаточно отладчика OllyDbg.

Чтобы найти сегмент, содержащий переменную `gLife` выполним следующие шаги:

1. Запустите отладчик OllyDbg. Нажмите F3, чтобы открыть диалог "Select 32-bit executable" (выберите 32-разрядный исполняемый файл). В диалоге выберите скомпилированное приложение из листинга 3-16. В резльтате отладчик запустит приложение и остановит его процесс на первой исполняемой инструкции процессора.

2. Нажмите комбинацию клавиш Ctrl+G, чтобы открыть диалог "Enter expression to follow" (ввести выражение для перехода).

3. Введите имена EXE модуля и функции `main` через точку в поле диалога "Enter address expression" (ввести адрес выражения). Должна получиться строка "TestApplication.main". После этого нажмите кнопку "Follow expression" (перейти к выражению). Теперь курсор окна дизассемблера должен указывать на первую инструкцию функции `main`.

4. Установите точкуостанова на эту инструкцию нажатием F2.

5. Начните исполнение процесса нажатием F9. Должна сработать наша точка останова.

6. Щёлкните правой кнопкой мыши по следующей строке дизассемблерного кода:
```assembly
MOV AX,WORD PTR DS:[gLife]
```
Позиция курсора должна совпадать с иллюстрацией 3-24.

![Test Application main](test-application-ollydbg.png)

**Иллюстрация 3-24.** *Точка останова в `main` функции*

7. Выберите пункт “Follow in Dump” ➤ “Memory address” ("Следить в дампе" ➤ "Адрес памяти") в открывшемся меню. Теперь курсор в окне дампа памяти указывает на переменную `gLife`. В моём случае она находится по адресу 329000 и имеет значение 14 в шестнадцатеричной системе.

8. Нажмите комбинацию клавиш Alt+M, чтобы открыть окно "Memory map" (карта памяти).

9. Найдите сегмент в котором находится переменная `gLife`. Это должен быть `.data` сегмент модуля TestApplication как на иллюстрации 3-25.

![Test Application сегмент .data](testapp-segment-ollydbg.png)

**Иллюстрация 3-25.** *Сегменты модуля TestApplication*

Мы выяснили, что переменная `gLife` хранится в самом начале сегмента `.data`. Следовательно, её адрес равен базовому адресу сегмента. Если бот найдет `.data`, он сразу сможет прочитать `gLife`.