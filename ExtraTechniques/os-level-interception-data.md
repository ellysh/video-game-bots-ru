# OS Level Interception Data

## Tools

We will work with WinAPI functions in this chapter. C++ language is the best choice for this task. We will use the [Visual Studio 2015 Community IDE](https://www.visualstudio.com/en-us/products/visual-studio-express-vs.aspx#) to compile our examples. More details about this IDE is available in the [In-game Bots](../InGameBots/tools.md) chapter.

There are several open source solutions, which can simplify the hooking of WinAPI calls.

First solution is [DLL Wrapper Generator](https://m4v3n.wordpress.com/2012/08/08/dll-wrapper-generator/). It can help us to create proxy DLLs.

These are steps to install DLL Wrapper Generator:

1. Download scripts from the github [project page](https://github.com/mavenlin/Dll_Wrapper_Gen/archive/master.zip).

2. Download and install [Python 2.7 version](https://www.python.org/downloads/)

Now we are ready to use the DLL Wrapper Generator.

Second solution is [Deviare](http://www.nektra.com/products/deviare-api-hook-windows/) open source hooking engine.

These are steps to install Deviare software:

1. Download the last version of the [release binaries](https://github.com/nektra/Deviare2/releases/download/v2.8.0/Deviare.2.8.0.zip).

2. Download the latest version of the [source code](https://github.com/nektra/Deviare2/archive/v2.8.0.zip).

3. Unpack both archives in two different directories.

You can find a list of all available Deviare releases in the [github project](https://github.com/nektra/Deviare2/releases). Please, make sure that the version of binaries matches to the version of sources.

## Test Application

We will use almost the same application to test WinAPI calls hooking techniques as we used in the [protection against in-game bots](../InGameBots/protection.md) section.

This is a source code of the [`TestApplication.cpp`](../Examples/ExtraTechniques/OSLevelInterceptionData/TestApplication.cpp):
```C++
#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include <string> 

static const uint16_t MAX_LIFE = 20;
volatile uint16_t gLife = MAX_LIFE;

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
        
        std::string str(gLife, '#');
        TextOutA(GetDC(NULL), 0, 0, str.c_str(), str.size());

        printf("life = %u\n", gLife);
        Sleep(1000);
    }
    printf("stop\n");
    return 0;
}
```
You can build the 32 bit version of this application and launch it.

Algorithm of this application stays the same. We decrement the `gLife` variable each second if the *1* keyboard key is not pressed. Otherwise, we increment the variable. New feature of the application is the call of the `TextOutA` WinAPI function. This function prints the hash symbols in the upper-left corner of the screen. Count of the printed symbols equals to the value of `gLife` variable.

Now our goal is to hook the `TextOutA` function call and to get its last parameter, which has the same value as the `gLife` variable. According to the WinAPI documentation the `TextOutA` function is provided by the `gdi32.dll` library.

## DLL Import

Before we start to consider WinAPI hooking, it will be useful to know how application interacts with DLL libraries. When we start an application, Windows loader reads an executable file into the process memory. Typical Windows executable file has [**PE**](https://msdn.microsoft.com/en-us/library/ms809762.aspx) format. This format is the standard for data structures, which are stored in a file header. These structures contain necessary information to launch executable code by the Windows loader. List of required DLLs is a part of this information.

Next step of the loader is to find files of all required DLLs on a disk drive. These files are read into the process memory too. Now we face an issue. Locations of the DLL modules in the process memory are not constant. The location can vary for different versions of the same DLL. Therefore, compiler cannot hardcode addresses of DLL functions in the executable module. This issue is solved by [**Import Table**](http://sandsprite.com/CodeStuff/Understanding_imports.html). There is some kind of confusion with Import Table and **Thunk Table**. Let us consider both these tables.

Each element of Import Table matches to one required DLL module. This element contains a name of the module, the `OriginalFirstThunk` pointer and the `FirstThunk` pointer. The `OriginalFirstThunk` points to the first element of the array with ordinal numbers and names of the imported functions. The `FirstThunk` points to the first element of the array (also known as **Import Address Table** or IAT), which is overwritten by the Windows loader with actual addresses of the imported functions. And this is a source of confusion because both these arrays do not contain any stuff that is named [**thunk**](https://en.wikipedia.org/wiki/Thunk).

You can find more details about both `OriginalFirstThunk` and `FirstThunk` pointers [here](http://ntcore.com/files/inject2it.htm). 

Import Table is a part of PE header and it contains constant meta information about imported DLLs. This table together with PE header occupies the read-only segment of the process memory. Thunk table (also known as a **jump table**) is a part of executable code and it contains `JMP` instructions to transfer control to the imported functions. This table occupies the read and executable `.text` segment together with all other application code. Import Address Table is stored in the read and write `.idata` segment. The `.idata` segment also contains an array, which is pointed by the `OriginalFirstThunk` pointer. As you see that all three tables are placed in different segments.

Some compilers generate a code, which does not use Thunk Table. This allows us to avoid one extra jump and to get slightly more optimized in runtime solution. Code, which is generated by the MinGW compiler, uses the Thunk Table. The scheme below illustrates a call of the `TextOutA` WinAPI function from this code:

![DLL call MinGW](dll-call-mingw.png)

This is an algorithm of the function call:

1. The `CALL` instruction performs two actions. It puts the return location to a stack and passes control to the Thunk Table element with the `40839C` address.

2. The Thunk Table element contains one `JMP` instruction only. This instruction uses the actual address of the `TextOutA` function in the `gdi32` module from the Import Address Table record. The `DS` segment register, which points to the `.idata` segment, is used to calculate address of this record:
```
DS + 0x278 = 0x422000 + 0x278 = 0x422278
```
3. The `TextOutA` function from the `gdi32` module is executed. There is a `RETN` instruction at the end of this function. The `RETN` passes control to the next instruction after the `CALL` one in the EXE module. It happens because the `CALL` instruction has put the return location to the stack.

The code, which is generated by the Visual C++ compiler, does not use Thunk Table. The scheme illustrates a call of the same `TextOutA` WinAPI function in this case:

![DLL call Visual C++](dll-call-visual-cpp.png)

This is an algorithm of the function call:

1. The `CALL` instruction passes control to the `TextOutA` function in the `gdi32` module directly. Address of this function is taken from the Import Address Table record.

2. The `TextOutA` function is executed. Then the `RETN` instruction passes control back to the EXE module.

## API Hooking Techniques

Game application interacts with Windows via system DLLs. Such operations as displaying a text on the screen are performed by WinAPI functions. It is possible to get a state of the game objects by hooking calls to these functions. This approach reminds the output device capture. But now we can analyze data before they come to the output device. These data can be pictures, sounds, network packets or a set of bytes in the temporary file.

You can see how API hooking works by launching the [API Monitor](../ClickerBots/tools.md) tool. This tool prints the hooked calls in the "Summary" sub-window. We can implement a bot application that behaves in the similar way. But unlike the API Monitor the bot should simulate player actions instead of printing hooked calls.

Now we will consider most common API hooking techniques with examples.

### Proxy DLL

First approach to hook WinAPI calls is to substitute original Windows library. We can implement a library that looks like the original one for the Windows loader point of view. Therefore, this library is loaded to the process memory during application launching. Then a game application interacts with the library in the same way as with the original one. This approach allows us to execute our code each time, when the game application calls any function from the WinAPI library. The library that can substitute original one is named **proxy DLL**.

We need to hook several specific WinAPI functions only in most cases. All other functions of the substituted Windows library are not interesting for us. Also there is a requirement: game application should behave with a proxy DLL in the same manner as it does with the original library. Therefore, the proxy DLL should route function calls to the original library. The functions, which should be hooked, can contain a code of the bot application to simulate player actions or to gather state of the game objects but the original WinAPI functions should be called after this code. We can make simple wrappers, which route to the original Windows library, for uninteresting for us functions. This means that the original library should be loaded in the process memory too. Otherwise, our wrappers will not work properly. The Windows loader loads the original libraries automatically because the proxy DLL depends on them.

This scheme illustrates a call of the `TextOutA` WinAPI function via a proxy DLL:

![Proxy DLL](proxy-dll.png)

This is an algorithm of the function call:

1. Windows loader loads the proxy DLL instead of the original gdi32.dll library. The Loader writes addresses of the functions, which are exported by the proxy DLL, to the Import Address Table of the EXE module.

2. Execution of the EXE module code reaches the `CALL` instruction. The record of Import Address Table is used to get the actual function address. Now this record contains the address of the proxy DLL function. The `CALL` instruction transfers control to the proxy DLL module.

3. Proxy DLL contains the Thunk Table. Addresses of its exported functions match to the thunks in this table. Therefore, the Thunk Table receives control from the `CALL` instruction of the executable module.

4. The `JMP` instruction of the thunk transfers control to the wrapper of the `TextOutA` WinAPI function, which is provided by proxy DLL. The wrapper contains bot's code.

5. The `CALL` instruction of the wrapper function passes control to the original `TextOutA` function of the `gdi32` module when the wrapper code is finished.

6. Original `TextOutA` function is executed. Then the `RETN` instruction transfers control back to the wrapper function.

7. The `RETN` instruction at the end of the `TextOutA` wrapper passes control back to the EXE module.

There is one question. How the proxy DLL knows actual addresses of the functions, which are exported by the `gdi32` module? We cannot delegate gathering of these addresses to the Windows loader. The problem is, the original Windows library should be loaded from the specific path. It means that we should avoid the library searching mechanism of Windows loader. Therefore, we should load the original library manually with the [`LoadLibrary`](https://msdn.microsoft.com/en-us/library/windows/desktop/ms684175%28v=vs.85%29.aspx) WinAPI function. The [`GetProcAddress`](https://msdn.microsoft.com/en-us/library/windows/desktop/ms683212%28v=vs.85%29.aspx) function helps us to dynamically get the actual addresses of its exported functions.

These are advantages of the proxy DLL approach:

1. It is easy to generate a proxy DLL with existing open source tools.

2. We substitute a Windows library for the specific application only. All other launched applications still uses the original libraries.

3. It is difficult to protect application against this approach.

These are disadvantages of the proxy DLL usage:

1. You cannot substitute some of core Windows libraries like `kernel32.dll`. This limitation appears because both `LoadLibrary` and `GetProcAddress` functions are provided by the `kernel32.dll`. They should be available at the moment when the proxy DLL loads the original library.

2. It is difficult to make wrappers for some WinAPI functions because they are not documented.

### Example of Proxy DLL

Now we will implement the simplest bot, which is based on the proxy DLL technique. The bot will keep the value of the `gLife` variable greater than 10. If the variable falls below 10, the bot simulates the *1* keypress.

First step to develop a proxy DLL is to generate source code of the library with wrapper functions. We can use the DLL Wrapper Generator script for this purpose. This is the algorithm to do it:

1. Copy the 32-bit version of the `gdi32.dll` library to the directory with the generator script. The library is located in the `C:\Windows\system32` directory for the 32-bit Windows and in the `C:\Windows\SysWOW64` for the 64-bit one.

2. Launch the `cmd.exe` Command Prompt application.

3. Launch the generator script via the command line:
```
python Generate_Wrapper.py gdi32.dll
```
You will get the Visual Studio project with generated wrapper functions. The project is located in the `gdi32` subdirectory. We will work with the 32-bit proxy DLL and 32-bit TestApplication to avoid confusion with its versions.

Second step is to adapt the generated proxy DLL for our purposes. This is a list of necessary changes in the library:

1. Open the `gdi32` Visual Studio project and answer "OK" in the "Upgrade VC++ Compiler and Libraries" dialog. This allows you to adapt the project to a new Visual Studio version.

2. Fix the path to the original `gdi32.dll` library in the `gdi32.cpp` source file. This path is specified in the line 10:
```C++
mHinstDLL = LoadLibrary( "ori_gdi32.dll" );
```
The path should be the same as one where you take the `gdi32.dll` library for DLL Wrapper Generator script. This is the example path for the 64-bit Windows case:
```C++
mHinstDLL = LoadLibrary( "C:\\Windows\\SysWOW64\\gdi32.dll" );
```
3. Substitute the wrapper of the `TextOutA` function to this implementation:
```C++
extern "C" BOOL __stdcall TextOutA_wrapper(
    _In_ HDC     hdc,
    _In_ int     nXStart,
    _In_ int     nYStart,
    _In_ LPCSTR lpString,
    _In_ int     cchString
    )
{
    if (cchString < 10)
    {
        INPUT Input = { 0 };
        Input.type = INPUT_KEYBOARD;
        Input.ki.wVk = '1';
        SendInput(1, &Input, sizeof(INPUT));
    }

    typedef BOOL(__stdcall *pS)(HDC, int, int, LPCTSTR, int);
    pS pps = (pS)mProcs[696];
    return pps(hdc, nXStart, nYStart, lpString, cchString);
}
```
Full version of the `gdi32.cpp` source file is available [here](../Examples/ExtraTechniques/OSLevelInterceptionData/gdi32.cpp).

Let us remember the TestApplication code that calls the `TextOutA` function to understand better our `TextOutA_wrapper` function. This is the code.
```
std::string str(gLife, '#');
TextOutA(GetDC(NULL), 0, 0, str.c_str(), str.size());
```
You can see that the length of the string with "#" symbols equals to the `gLife` variable. The length is the last parameter of the `TextOutA_wrapper` function with the `cchString` name. We compare the parameter value with 10 and simulate the keypress with the `SendInput` WinAPI function if the comparison fails. After this we call the original `TextOutA` function via its pointer. The `mProcs` array contains the pointers to all function of the original `gdi32.dll` library. We fill this array in the `DllMain` function when the proxy DLL is loaded.

The `TextOutA_wrapper` function was looking like this before we did our changes:
```C++
extern "C" __declspec(naked) void TextOutA_wrapper(){__asm{jmp mProcs[696*4]}}
```
There is a question, why we use the "696*4" index of the `mProcs` array in the original wrapper and the "696" index in our implementation? This happens because indexing in assembler is performed in bytes. Each element of the `mProcs` array is a pointer to the function. Pointers have the 4 bytes (or 32 bits) size for the 32-bit architecture. This is the reason, why we multiply array's index to 4 for the `jmp` assembler instruction. C++ language uses information about a type of the array elements to calculate their offsets correctly.

Third step is to prepare the environment to use proxy DLL:

1. Build the 32-bit version of the `gdi32.dll` proxy DLL.

2. Copy the `gdi32.dll` proxy DLL to the directory with the `TestApplication.exe` executable file.

3. Add the `gdi32.dll` system library to the `ExcludeFromKnownDLL` key register. This is the path to the key:
```
HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\Session Manager\ExcludeFromKnownDlls
```
4. Reboot your computer for the register change to take effect.

Windows has some kind of [protection mechanism](https://support.microsoft.com/en-us/kb/164501) that prevents malware to substitute system libraries. This mechanism implies to list all important libraries in the register. These libraries are able to be load from the predefined system paths only. There is the special `ExcludeFromKnownDLL` register key that allows us to avoid this protection mechanism. We add the `gdi32.dll` library in the exclude list. Now the loader uses the standard search order for this library. The loader starts its searching from the current directory. Therefore, the proxy DLL will be loaded instead of the original library.

Now you can launch the `TestApplication.exe` file. You will see in console that the `gLife` variable does not fall below 10.

### API Patching

Second approach to hook a WinAPI call is to modify a function itself. When Windows library is loaded into the memory of a target process, we can access the memory and modify it.

There are several ways, how we can overwrite the beginning of the API function to hook it. The most common approach is to write the control transfer assembler instructions like `CALL` or `JMP`. These instructions pass control to our handler function immediately after the call of the WinAPI function.

Next task is to execute original API function when our handler finishes its work. As you remember, the beginning of the original function was overwritten. Thus, we should restore it in our handler. Otherwise, we will get recursive calls of the handler, which lead to stack overflow and application crash. When the original function finishes, we can patch its beginning again. It allows us to be prepared to hook a next call of the original function.

There is a question: how we can modify the behavior of the target application? This application works with the original WinAPI libraries and do not patch them. The answer is we should inject our code into the target application. This code will patch the original API functions to call our handlers. These code injection techniques are mentioned in the [Example with Diablo 2](../InGameBots/example.md) section.

This is a [code snippet](https://en.wikipedia.org/wiki/Hooking#API.2FFunction_Hooking.2FInterception_Using_JMP_Instruction) with a possible implementation of this technique.

This scheme illustrates the way to handle `TextOutA` WinAPI function with API patching technique:

![API Patching](api-patching.png)

This is an algorithm, how the WinAPI call happen now:

1. The "handler.dll" module was injected to the target application. When this module is loaded, it patches the beginning of the `TextOutA` WinAPI function.

2. Execution of the EXE module reaches the `CALL` instruction. The Import Address Table is used to retrieve the address of the `TextOutA` function. The `CALL` instruction pass control to the "gdi32.dll" module.

3. First instruction of the `TextOutA` function was patched. There is a `JMP` instruction, which passes control to the "handler.dll" module.

4. The "handler.dll" module does his hooking job and then call the original `TextOutA` function. This step is needed to keep the target application working. The application expects that the original WinAPI function will be performed.

5. The `RETN` instruction at the end of the handler function passes control back to the EXE module.

These are advantages of the API patching approach:

1. You can hook calls from the Windows core libraries.

2. There are several frameworks, which automate the low level tasks to inject code and patching WinAPI functions.

These are disadvantages of the API patching:

1. Size of the hooked functions should be greater than 5 bytes. Otherwise, we cannot patch a beginning of the function with the `JMP` instruction.

2. It is difficult to implement this technique on your own. You should pay attention to avoid the infinite recursive calls.

3. This technique is not reliable for multithread applications. WinAPI function will not be hooked until the handler module is processing the call. It happens because beginning of the function is restored at this moment.

### Example of API Patching

Now we will consider the bot application, which uses the API patching technique. Our application will be based on the Deviare hooking engine. We will test our bot with the same TestApplication that we have used before to consider the proxy DLL technique.

First of all let us review basic features of the Deviare engine. Distribution of this engine contains several sample applications. They demonstrate its basic features. CTest is one of these sample applications. It allow us to hook calls of WinAPI functions and store details about these calls in the text log file.

This is an algorithm to launch the CTest sample application together with our TestApplication:

1. Download the [release binaries](https://github.com/nektra/Deviare2/releases/download/v2.8.0/Deviare.2.8.0.zip) of the Deviare engine if you did not do it yet. Unpack this archive to your local disc. Let us name this directory as the `deviare-bin`.

2. Copy the `TestApplication.exe` executable file to the `deviare-bin` directory.

3. Open the [`ctest.hooks.xml`](../Examples/ExtraTechniques/OSLevelInterceptionData/ctest.hooks.xml) configuration file. This file contains a list of WinAPI functions to hook. You should add the `TextOutA` function into this list. This task will be solved if you put this line between the `<hooks>` and </hooks> tags:
```
<hook name="TextOutA">gdi32.dll!TextOutA</hook>
```

4. Launch the CTest application with these command line parameters:
```
CTest.exe exec TestApplication.exe -log=out.txt
```
The `exec` parameter means that the target application with the specified executable file will be launched.
The `-log` parameter allows us to specify the log file for CTest output.

You can use the standard `cmd.exe` Windows utility to launch applications with parameters. When you launch the CTest sample, you will see its window and the window of our TestApplication. The `gLife` variable is decreasing until 0. You can interrupt the CTest application by the *Ctrl+C* keypress after the TestApplication has finished its work. 

Now we have the `out.txt` file with all information that was gathered by CTest. Let us consider this file in details. You can find these lines in the log file:
```
CNktDvEngine::CreateHook (gdi32.dll!TextOutA) => 00000000
...
21442072: Hook state change [2500]: gdi32.dll!TextOutA -> Activating
...
21442306: LoadLibrary [2500]: C:\Windows\System32\gdi32.dll / Mod=00000003
...
21442852: Hook state change [2500]: gdi32.dll!TextOutA -> Active
```
If your log file contains these lines, it means that the hook for the `TextOutA` function has been set and activated successfully. Below these lines you should find details about each function call. This is an example for my case:
```
21442852: Hook called [2500/2816 - 1]: gdi32.dll!TextOutA (PreCall)
     [KT:15.600100ms / UT:0.000000ms / CC:42258224]
21442852:   Parameters:
              HDC hdc [0x002DFA60] "1795229328" (unsigned dword)
              long x [0x002DFA64] "0" (signed dword)
              long y [0x002DFA68] "0" (signed dword)
              LPCSTR lpString [0x002DFA6C] "#" (ansi-string)
              long c [0x002DFA70] "19" (signed dword)
21442852:   Custom parameters:
21442852:   Stack trace:
21442852:     1) TestApplication.exe + 0x00014A91
21442852:     2) TestApplication.exe + 0x0001537E
21442852:     3) TestApplication.exe + 0x000151E0
21442852:     4) TestApplication.exe + 0x0001507D
```
You can see that Deviare engine allows us to get information about type and value of each parameter of the hooked function. This is totally enough for our sample bot application. But also Deviare knows about the exact time, when the function was called, and the full stack trace. The stack trace can help us to distinguish the WinAPI function call that should be processed by the bot from one that should be ignored.

Our second step is to adapt CTest sample to behave as a bot application. We can implement the same algorithm that we have done for proxy DLL sample. When CTest hook the `TextOutA` function call, it should simulate the *1* keypress if the `gLife` variable is below 10.

To modify the CTest application you should open the Visual Studio project file in the Deviare sources with the `Samples\C\Test\CTest.sln` path. The algorithm of the hooked functions processing is implemented in the `MySpyMgr.cpp` file. You can find the  `CMySpyMgr::OnFunctionCalled` method in this file. This method is called by Deviare engine before to pass control to the hooked WinAPI function. Now this method of the `CMySpyMgr` class contains calls of the `LogPrint` function only. This is how the CTest application puts gathered information to the resulting log file.

We should add the behavior of our bot application into this `CMySpyMgr::OnFunctionCalled` method. We can do it in two steps. First step is to add the function that will process the last parameter of the `TextOutA` WinAPI function. As you remember, this parameter matches to the length of the string to print, i.e. value of the `gLife` variable. This is a source code of the `ProcessParam` function:
```C++
VOID ProcessParam(__in Deviare2::INktParam *lpParam)
{
    CComBSTR cBstrTypeName, cBstrName;
    lpParam->get_Name(&cBstrName);

    unsigned long val = 0;
    HRESULT hRes = lpParam->get_ULongVal(&val);
    if (FAILED(hRes))
        return;

    wprintf(L"ProcessParam() - name = %s value = %u\n", (BSTR)cBstrName, (unsigned int)(val));

    if (val < 10)
    {
        INPUT Input = { 0 };
        Input.type = INPUT_KEYBOARD;
        Input.ki.wVk = '1';
        SendInput( 1, &Input, sizeof( INPUT ) );
    }
}
```
This is an algorithm of this function:

1. Read an integer value of the passed parameter of the function with the `get_ULongVal` method. If this read fails, we return from the `ProcessParam` function.

2. Print name of the parameter and its value.

3. Check the parameter value. Simulate the *1* keypress if the value is less than 10.

Second step is to call the `ProcessParam` function from the `CMySpyMgr::OnFunctionCalled` method. You can find these lines in the method:
```C++
    if (sCmdLineParams.bAsyncCallbacks == FALSE &&
        SUCCEEDED(callInfo->Params(&cParameters)))
    {
        LogPrint(L"  Parameters:\n");
```
If you check the log file, you will find there the "Parameters:" lines. It means that the CTest application starts to analyze parameters of the hooked function in this place of the code. We should add the call of our `ProcessParam` function here:
```C++
    if (sCmdLineParams.bAsyncCallbacks == FALSE &&
        SUCCEEDED(callInfo->Params(&cParameters)))
    {
        if (SUCCEEDED(cParameters->GetAt(4, &cParam)))
            ProcessParam(cParam);

        LogPrint(L"  Parameters:\n");
```
We get the parameter object, which matches to the life parameter, in the added `if` condition. If the `GetAt` method succeed, we call our `ProcessParam` function with the extracted parameter object. First argument of the `GetAt` method is a number of the parameter in the hooked function. You can clarify this number in the [documentation](https://msdn.microsoft.com/en-us/library/windows/desktop/dd145133(v=vs.85).aspx). Do not forget that counting of function parameters starts from number 0.

This is the resulting [`MySpyMgr.cpp`](../Examples/ExtraTechniques/OSLevelInterceptionData/MySpyMgr.cpp) file after our patches. You should build the patched version of the CTest application. You can find the resulting executable file in the `bin` directory of the Deviare source tree.

These are steps to launch CTest and TestApplication together:

1. Copy the patched `CTest.exe` executable file to the directory with the Deviare binaries. We have named this directory as `deviare-bin` when we tested the original CTest application.

2. Copy the `TestApplication.exe` executable file to the `deviare-bin` directory.

3. Launch the CTest application:
```
CTest.exe exec TestApplication.exe -log=out.txt
```
You will see windows of both CTest and TestApplication after launch of this command:

![API Patching Example](api-patching-example.png)

You can see an actual value of the `gLife` variable in the TestApplication window. This value is printed to the console. The same value, which was got from the hooked `TextOutA` function parameter, is printed in the CTest application window. You can see that the `gLife` value is increased each time, when it falls below 10.

## Summary

We have considered only two approaches to hook WinAPI function calls in this chapter. There are proxy DLL and API patching techniques. You can learn about other approaches in this [article](http://www.internals.com/articles/apispy/apispy.htm).
