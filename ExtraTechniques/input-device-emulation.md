# Input Device Emulation

## Tools

First of all we should choose hardware to emulate input devices. This is a list of hardware features, which are important for our purposes:

1. Low price of the device.
2. IDE and compiler should be available for free.
3. IDE should provide libraries for emulation input devices.
4. Active project community and good documentation.

Arduino board has all of these features. This hardware is a good choice to start your first experiments with embedded development.

Next question is which version of Arduino board you should buy. Arduino IDE provides [libraries](https://www.arduino.cc/en/Reference/MouseKeyboard) to emulate keyboard and mouse devices. According to the documentation these libraries are not supported by some boards. Appropriate versions of boards for you are Leonardo, Micro and Due. When you get a board, you should connect it to your computer via USB cable. Now the hardware is ready to work.

Second topic after preparing the hardware is to choose a software for development. Arduino IDE with integrated C++ compiler and libraries is available for download on the [official website](http://www.arduino.org/downloads). You should download it and install.

Next step is to install drivers for Arduino board. You should launch the installer application from the Arduino IDE subdirectory. This is a default path for the installer `C:\Program Files (x86)\Arduino\drivers`. There are two installers with `dpinst-amd64.exe` and `dpinst-x86.exe` names in the `drivers` directory. You should choose the first installer for 64-bit Windows version and the second one for 32-bit version. The board should be connected to your computer during drivers installation.

There are steps to configure Arduino IDE after installation:

1. Choose a model of your board as a target device for compiler. This option is available in the "Tools"->"Board:..." item of the main menu. You can get correct name of your model from the "Tools"->"Port:..." menu item.

2. Choose a connection port to the board via the "Tools"->"Port:..." item of the main menu.

Now Arduino IDE is prepared and you can start to program.

We will use AutoIt scripting language to communicate with Arduino board. Therefore, you need [CommAPI scripts](https://www.autoitscript.com/wiki/CommAPI), which provide access to the WinAPI communications functions. This is a [mirror](https://github.com/ellysh/CommAPI) with all CommAPI scripts gathered into one archive.

## Keyboard Emulation

There are several ways to develop a bot application, which is based on emulator of input devices.

The first possibility is to implement all bot's algorithms into the application for Arduino board. When you upload this application to the device, the bot is ready to work. It will be launched automatically each time when you connect the board to a computer. This way is appropriate to get a blind clicker bot. This kind of bot simulates user actions with fixed time delays in the infinite loop. It does not have any information about a game state. But if you want to make a bot, which is able to analyze what happens, you should choose another approach. The problem is Arduino board itself does not have any possibility to access the screen device or memory of the game process to get any information about it.

Second way is to write an application for Arduino board, which is able to receive commands via [serial port](https://en.wikipedia.org/wiki/Serial_port) and simulate user actions according to these commands. In this case we can implement a clicker bot application, which analyzes a picture of the game window and performs appropriate actions with an emulator. We will consider this way here as more universal and flexible one.

Our first example Arduino application simulates keyboard actions according to the received bytes. It just receives bytes via serial interface and send them back as keypress actions.

This is source code of the [`keyboard.ino`](https://ellysh.gitbooks.io/video-game-bots/content/Examples/ExtraTechniques/InputDeviceEmulation/keyboard.ino) application:
```C++
#include <Keyboard.h>

void setup()
{
  Serial.begin(9600);
  Keyboard.begin();
}

void loop()
{
  if (Serial.available() > 0)
  {
    int incomingByte = Serial.read();
    Keyboard.write(incomingByte);
  }
}
```
Let us consider this application in details. We use a standard library with the **Keyboard** name here. This library allows us to send keystrokes to the connected computer via [**HID**](https://en.wikipedia.org/wiki/Human_interface_device) protocol. We include the `Keyboard.h` header at the first line of the application. The `Keyboard_` class is defined in this header and the `Keyboard` global object is created. The `Keyboard` object provides an access to features of the library.

There are two functions with `setup` and `loop` names in our application. When you compile any Arduino application, the IDE adds the default `main` function implicitly. This `main` function calls the `setup` function once at startup. Then the `loop` function is called repeatedly. [Signatures](http://stackoverflow.com/questions/2322736/what-is-the-difference-between-function-declaration-and-signature) of both `setup` and `loop` functions are predefined and you cannot change them.

We initialize both `Serial` and `Keyboard` objects in the `setup` function. The baud rate parameter, which equals to 9600 bit/s, is passed to the [`begin`](https://www.arduino.cc/en/Serial/Begin) method of the `Serial` object. This parameter defines the data transfer rate between the Arduino board and the connected computer. The [`begin`](https://www.arduino.cc/en/Reference/KeyboardBegin) method of the `Keyboard` object does not have any input parameters. Now the serial communication and the keyboard emulation are ready to work.

There are three actions in the `loop` function:

1. Check if the data was received via the serial port with the [`available`](https://www.arduino.cc/en/Serial/Available) method of the `Serial` object. This method returns the number of received bytes.

2. Read one received byte by the [`read`](https://www.arduino.cc/en/Serial/Read) method of the `Serial` object. This byte defines an ASCII code of the key, which should be simulated.

3. Send a keystroke action to the connected computer with the [`write`](https://www.arduino.cc/en/Reference/KeyboardWrite) method of the `Keyboard` object.

You should press the *Ctrl+U* hotkey in Arduino IDE to compile and upload the `keyboard.ino` application to the Arduino board.

Now we have the Arduino board, which emulates the keyboard. Next step is to implement an AutoIt script to control this board via the serial port. This control script uses CommAPI wrappers. You should download all CommAPI files and copy them to script's directory.

This is a list of necessary CommAPI files:

1. [`CommAPI.au3`](https://www.autoitscript.com/wiki/CommAPI.au3)
2. [`CommAPIConstants.au3`](https://www.autoitscript.com/wiki/CommAPIConstants.au3)
3. [`CommAPIHelper.au3`](https://www.autoitscript.com/wiki/CommAPIHelper.au3)
4. [`CommInterface.au3`](https://www.autoitscript.com/wiki/CommInterface.au3)
5. [`CommUtilities.au3`](https://www.autoitscript.com/wiki/CommUtilities.au3)

Make sure that all of these files are present.

Our example control script will ask emulator to print the "Hello world!" string. This is a source code of this [`ControlKeyboard.au3`](https://ellysh.gitbooks.io/video-game-bots/content/Examples/ExtraTechniques/InputDeviceEmulation/ControlKeyboard.au3) script:
```AutoIt
#include "CommInterface.au3"

func ShowError()
	MsgBox(16, "Error", "Error " & @error)
endfunc

func OpenPort()
	local const $iPort = 7
	local const $iBaud = 9600
	local const $iParity = 0
	local const $iByteSize = 8
	local const $iStopBits = 1

	$hPort = _CommAPI_OpenCOMPort($iPort, $iBaud, $iParity, $iByteSize, $iStopBits)
	if @error then
		ShowError()
		return NULL
	endif
 
	_CommAPI_ClearCommError($hPort)
	if @error then
		ShowError()
		return NULL
	endif
 
	_CommAPI_PurgeComm($hPort)
	if @error then
		ShowError()
		return NULL
	endif
	
	return $hPort
endfunc

func SendArduino($hPort, $command)
	_CommAPI_TransmitString($hPort, $command)
	if @error then ShowError()
endfunc

func ClosePort($hPort)
	_CommAPI_ClosePort($hPort)
endfunc

$hWnd = WinGetHandle("[CLASS:Notepad]")
WinActivate($hWnd)
Sleep(200)

$hPort = OpenPort()

SendArduino($hPort, "Hello world!")

ClosePort($hPort)
```
This is an algorithm of the script:

1. Switch to the Notepad window with the `WinActivate` AutoIt function.

2. Open the serial port with the `OpenPort` function.

3. Send command to the Arduino board to type the "Hello world!" string. The `SendArduino` function encapsulates algorithm of sending this command.

4. Close the serial port with the `ClosePort` function.

Let us consider internals of `OpenPort`, `SendArduino` and `ClosePort` user functions.

The `OpenPort` function opens the serial port and prepare the connected device for communication. This function returns a handle to the opened port. Three CommAPI functions are used here:

1. The `_CommAPI_OpenCOMPort` function opens a COM port with the specified settings. These settings are passed via input parameters of the function. The `iParity`, `iByteSize` and `iStopBits` parameters have constant values for serial connections with any Arduino board. You should pay attention to the `iBaud` and `iPort` parameters only. The value of `iBaud` parameter should match to the value that you have passed to the `begin` method of the `Serial` object in the `keyboard.ino` application. This equals to 9600 in our case. The `iPort` parameter should be equal to the number of a COM port, which is used to connect Arduino board with your computer. You can check this value in the "Tools"->"Port:..." item of the Arduino IDE menu. For example, value `7` of the `iPort` parameter matches to the "COM7" port.

2. The `_CommAPI_ClearCommError` function retrieves information about communication errors and current status of the connected board. This information is returned via the second parameter of the function. This parameter is not used in our case. The function is used here to clear the error flag on the device. Communication will be blocked until this flag is not cleared.

3. The `_CommAPI_PurgeComm` function clears the input and output buffers of the board and terminate all pending read and write operations. The device becomes ready to receive commands after call of this function.

The `SendArduino` function is a wrapper around the `_CommAPI_TransmitString`. This function writes a string to the specified port handle.

The `ClosePort` function closes the serial port by the specified handle.

Also there is a `ShowError` function, which is used to show a message box with the code of last occurred error.

You can connect Arduino board with `keyboard.ino` application, launch Notepad and start the `ControlKeyboard.au3` script. The text "Hello world!" will be typed in the Notepad window.

## Keyboard Modifiers

Our `keyboard.ino` Arduino application is able to simulate presses of single keys. But this application does not allow us to simulate combination of keys for example *Ctrl+Z*. Let us improve it and AutoIt control script to provide this feature.

This is improved version of the Arduino application with the [`keyboard-combo.ino`](https://ellysh.gitbooks.io/video-game-bots/content/Examples/ExtraTechniques/InputDeviceEmulation/keyboard-combo.ino) name:
```C++
#include <Keyboard.h>

void setup()
{
  Serial.begin(9600);
  Keyboard.begin();
}

void pressKey(char modifier, char key)
{
  Keyboard.press(modifier);
  Keyboard.write(key);
  Keyboard.release(modifier);
}

void loop()
{
  static const char PREAMBLE = 0xDC;
  static const uint8_t BUFFER_SIZE = 3;

  if (Serial.available() > 0)
  {
    char buffer[BUFFER_SIZE] = {0};
    uint8_t readBytes = Serial.readBytes(buffer, BUFFER_SIZE);

    if (readBytes != BUFFER_SIZE)
      return;

    if (buffer[0] != PREAMBLE)
      return;

     pressKey(buffer[1], buffer[2]);
  }  
}
```
Here we use the [`readBytes`](https://www.arduino.cc/en/Serial/ReadBytes) method of the `Serial` object. This method allows us to read sequence of bytes, which are received via the serial port. The method returns an actual number of the received bytes.

Now each command of the control AutoIt script consists of three bytes. The first byte is a [**preamble**](https://en.wikipedia.org/wiki/Syncword). This is a predefined byte, which signals about a start of the command. The second byte is a code of the [key modifier](https://www.arduino.cc/en/Reference/KeyboardModifiers). This modifier should be pressed together with the key. The third byte is a code of the key. For example, if you want to simulate the *Alt+Tab* key combination, the command for Arduino board looks like this in hex:
```
0xDC 0x82 0xB3
```
The "0xDC" byte is a preamble. The "0x82" is a code of modifier, which matches to the left *Alt* key. The "0xB3" is a code of the *Tab* key.

You can see that the `loop` function has two `if` conditions, which interrupt a processing of the received command. The first condition validates a number of received bytes and the second one checks correctness of the preamble. If both checks are passed, the `pressKey` function is called. There are two parameters of this function: codes of modifier and key. The [`press`](https://www.arduino.cc/en/Reference/KeyboardPress) method of the `Keyboard` object is used here to hold a modifier until the key is pressing. The [`release`](https://www.arduino.cc/en/Reference/KeyboardRelease) method is used to release the modifier.

The control script should be adapted to the new protocol of the Arduino application. This is a new version of the script with the [`ControlKeyboardCombo.au3`](https://ellysh.gitbooks.io/video-game-bots/content/Examples/ExtraTechniques/InputDeviceEmulation/ControlKeyboardCombo.au3) name:
```AutoIt
#include "CommInterface.au3"

func ShowError()
	MsgBox(16, "Error", "Error " & @error)
endfunc

func OpenPort()
	; This function is the same as one in the ControlKeyboard.au3 script
endfunc

func SendArduino($hPort, $modifier, $key)
	local $command[3] = [0xDC, $modifier, $key]
	
	_CommAPI_TransmitString($hPort, StringFromASCIIArray($command, 0, UBound($command), 1))

	if @error then ShowError()
endfunc

func ClosePort($hPort)
	_CommAPI_ClosePort($hPort)
	if @error then ShowError()
endfunc

$hPort = OpenPort()

SendArduino($hPort, 0x82, 0xB3)

ClosePort($hPort)
```
There is only one change in the `SendArduino` function comparing to the `ControlKeyboard.au3` script. Now we transfer a `$command` array to the Arduino board. This array contains three bytes: preamble, modifier and key. The same `_CommAPI_TransmitString` function as before is used here to transmit data via the serial port. We use the [`StringFromASCIIArray`](https://www.autoitscript.com/autoit3/docs/functions/StringFromASCIIArray.htm) AutoIt function to convert `$command` array to the string format. This format is required by the `_CommAPI_TransmitString` function.

You can upload the new Arduino application to the board and launch the `ControlKeyboardCombo.au3` script. The *Alt+Tab* keystroke will be emulated. If you have several opened windows on your desktop, these windows will be switched by this keystroke.

## Mouse Emulation

Arduino board can emulate mouse device in the same way as keyboard one. The **Mouse** library of Arduino IDE provides this feature. But this library was designed to develop devices similar to mouse, which are based on Arduino board. This is a reason why the library uses relative coordinates for cursor positioning. This means that you can specify where to move the cursor from the current position. Operation with relative coordinates is not appropriate for bot development.

This [article](http://forum.arduino.cc/index.php?topic=94140.0) describes a way to patch the **HID** Arduino IDE library. This patch allows us to operate with absolute cursor coordinates. The described approach is suitable for old 1.0 version of Arduino IDE where both Keyboard and Mouse libraries were gathered together into one HID library.

There is an algorithm to patch Mouse library, if you use newer version of the Arduino IDE:

1. Download the patched [`Mouse.cpp`](https://ellysh.gitbooks.io/video-game-bots/content/Examples/ExtraTechniques/InputDeviceEmulation/Mouse.cpp) source file.

2. Substitute the original `Mouse.cpp` file in the Arduino IDE directory by the patched one. This is a default path to this file: `C:\Program Files (x86)\Arduino\libraries\Mouse\src`.

There are changes in the patched `Mouse.cpp` file:
```C++
#define ABSOLUTE_MOUSE_MODE

static const uint8_t _hidReportDescriptor[] PROGMEM = {
...
#ifdef ABSOLUTE_MOUSE_MODE
    0x15, 0x01,                    //     LOGICAL_MINIMUM (1)
    0x25, 0x7F,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
#else
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
#endif
```
We have changed the `_hidReportDescriptor` byte array. This [**report descriptor**](https://www.circuitsathome.com/communicating-arduino-with-hid-devices-part-1) declares data that device sends to the computer and data that can be sent to the device. In other words, report descriptor defines the communication protocol between computer and device. Representation of report descriptor in standard form allows computer to communicate with all HID devices in one universal way.

We made two changes in the report descriptor of mouse device:

1. The `LOGICAL_MINIMUM` value was changed from -127 value to 1. This is needed because negative absolute coordinates are not allowed.

2. The `INPUT` value was changed from `0x81, 0x06` to `0x81, 0x02`. This means that absolute coordinates are used instead of the relative ones.

The `ABSOLUTE_MOUSE_MODE` macro allows you to switch between usage of relative and absolute coordinates. If the macro is defined, the absolute cursor coordinates are used.

This is an Arduino application with [`mouse.ino`](https://ellysh.gitbooks.io/video-game-bots/content/Examples/ExtraTechniques/InputDeviceEmulation/mouse.ino) name, which simulates mouse clicks on the screen:
```C++
#include <Mouse.h>

void setup()
{
  Serial.begin(9600);
  Mouse.begin();
}

void click(signed char x, signed char y, char button)
{
  Mouse.move(x, y);
  Mouse.click(button);
}

void loop()
{
  static const char PREAMBLE = 0xDC;
  static const uint8_t BUFFER_SIZE = 4;

  if (Serial.available() > 0)
  {
    char buffer[BUFFER_SIZE] = {0};
    uint8_t readBytes = Serial.readBytes(buffer, BUFFER_SIZE);

    if (readBytes != BUFFER_SIZE)
      return;

    if (buffer[0] != PREAMBLE)
      return;

   click(buffer[1], buffer[2], buffer[3]);
  }
}
```
Algorithm of this application is similar to the `keyboard-combo.ino` one. Here we include the `Mouse.h` header instead of the `Keyboard.h` one. This header provides the `Mouse_` class and the `Mouse` global object. The same [`begin`](https://www.arduino.cc/en/Reference/MouseBegin) method is called in the `setup` function to initialize the `Mouse` object.

Mouse click simulation happens in the `click` function. There are two actions in this function. The first one is moving cursor to the specified position. We use the [`move`](https://www.arduino.cc/en/Reference/MouseMove) method of `Mouse` object to do it. The second action is a click simulation in the current cursor position with the [`click`](https://www.arduino.cc/en/Reference/MouseClick) method.

The commands from computer are processed in the `loop` function. Meaning of received bytes differs comparing to the `keyboard-combo.ino` application. Now the control AutoIt script sends four bytes:

1. Preamble
2. X coordinate of the click action.
3. Y coordinate of the click action.
4. Button to click.

You have noticed that the maximum value of both X and Y coordinates equals 127. The 127 or 0x7F value is a maximum signed number that can be stored in one byte. But your screen resolution should be much more than 127x127 pixels. You can convert actual cursor coordinates in pixels of your screen resolution to Arduino representation. There are a formulas to calculate Arduino coordinates:
```
Xa = 127 * X / Xres
Ya = 127 * Y / Yres
```
| Symbol | Description |
| -- | -- |
| Xa | X coordinate of point in Arduino representation |
| Ya | Y coordinate of point in Arduino representation |
| X | X actual coordinate of point in pixels |
| Y | Y actual coordinate of point in pixels |
| Xres | Horizontal screen resolution in pixels |
| Yres | Vertical screen resolution in pixels |

My screen resolution is 1366x768. This is an example of calculation Arduino coordinates of point x=250 and y=300 for my case:
```
Xa = 127 * 250 / 1366 = 23
Ya = 127 * 300 / 768 = 49
```
This means that if I want to simulate mouse click in point x=250 and y=300, I should send this command to the Arduino board:
```
0xDC 0x17 0x31 0x1
```
The 0x17 in hexadecimal equals 23 in decimal and 0x31 equals 49 similarly.

This is a [`ControlMouse.au3`](https://ellysh.gitbooks.io/video-game-bots/content/Examples/ExtraTechniques/InputDeviceEmulation/ControlMouse.au3) script to communicate with the `mouse.ino` application:
```AutoIt
#include "CommInterface.au3"

func ShowError()
	MsgBox(16, "Error", "Error " & @error)
endfunc

func OpenPort()
	; This function is the same as one in the ControlKeyboard.au3 script
endfunc

func GetX($x)
	return (127 * $x / 1366)
endfunc

func GetY($y)
	return (127 * $y / 768)
endfunc

func SendArduino($hPort, $x, $y, $button)
	local $command[4] = [0xDC, GetX($x), GetY($y), $button]

	_CommAPI_TransmitString($hPort, StringFromASCIIArray($command, 0, UBound($command), 1))

	if @error then ShowError()
endfunc

func ClosePort($hPort)
	_CommAPI_ClosePort($hPort)
	if @error then ShowError()
endfunc

$hWnd = WinGetHandle("[CLASS:MSPaintApp]")
WinActivate($hWnd)
Sleep(200)

$hPort = OpenPort()

SendArduino($hPort, 250, 300, 1)

ClosePort($hPort)
```
This script is very similar to the `ControlKeyboardCombo.au3` one. Now the `SendArduino` function have four input parameters: port number, cursor coordinates and button to click. Also there are `GetX` and `GetY` functions to convert cursor coordinates to the Arduino representation.

You can upload the `mouse.ino` application to Arduino board, launch Paint application and launch the `ControlMouse.au3` script. The script simulates left button click at the point with x=250 y=300 absolute coordinates in the Paint window.

## Keyboard and Mouse Emulation

One Arduino board is able to emulate both keyboard and mouse devices at the same time. Now we will consider application that simulates keystrokes and mouse clicks according to the received command from the control script. This application should combine approaches of the `mouse.ino` and `keyboard-combo.ino`, which are considered before.

This is the resulting [`keyboard-mouse.ino`](https://ellysh.gitbooks.io/video-game-bots/content/Examples/ExtraTechniques/InputDeviceEmulation/keyboard-mouse.ino) Arduino application:
```C++
#include <Mouse.h>
#include <Keyboard.h>

void setup()
{
  Serial.begin(9600);
  Keyboard.begin();
  Mouse.begin();
}

void pressKey(char key)
{
  Keyboard.write(key);
}

void pressKey(char modifier, char key)
{
  Keyboard.press(modifier);
  Keyboard.write(key);
  Keyboard.release(modifier);
}

void click(signed char x, signed char y, char button)
{
  Mouse.move(x, y);
  Mouse.click(button);
}

void loop()
{
  static const char PREAMBLE = 0xDC;
  static const uint8_t BUFFER_SIZE = 5;
  enum
  {
    KEYBOARD_COMMAND = 0x1,
    KEYBOARD_MODIFIER_COMMAND = 0x2,
    MOUSE_COMMAND = 0x3
  };
  
  if (Serial.available() > 0)
  {
    char buffer[BUFFER_SIZE] = {0};
    uint8_t readBytes = Serial.readBytes(buffer, BUFFER_SIZE);
    
    if (readBytes != BUFFER_SIZE)
      return;

    if (buffer[0] != PREAMBLE)
      return;

    switch(buffer[1])
    {
      case KEYBOARD_COMMAND:
        pressKey(buffer[3]);
        break;

      case KEYBOARD_MODIFIER_COMMAND:
        pressKey(buffer[2], buffer[3]);
        break;

      case MOUSE_COMMAND:
        click(buffer[2], buffer[3], buffer[4]);
        break;
    }
  }
}
```
Now the control script sends command that contains five bytes. The first byte is a preamble. The second byte is a code of the action that should be performed by the application:

| Code | Simulated Action |
| -- | -- |
| 0x1 | Keystroke without a modifier |
| 0x2 | Keystroke with a modifier |
| 0x3 | Mouse click |

Either the `pressKey` or `click` function will be called depending of this action code.

You can see that only three or four bytes are needed for keystroke actions. Why we transmit extra bytes for these commands? The reason of this decision is a behavior of the `readBytes` method of the `Serial` object. The problem is we should specify exact count of bytes that we want to read. But we do not know which of two commands will be received next.

There are several solutions of this problem:

1. Use commands of fixed size. We use this approach in our example.

2. Receive bytes with the [`readBytesUntil`](https://www.arduino.cc/en/Serial/ReadBytesUntil) method of the `Serial` object. This approach leads to transfer one extra terminator byte, which signals about the command end.

3. Read bytes with the `read` command of the `Serial` object. This approach allows us to detect an action code and a command length after receiving the second byte. But this way is less reliable comparing to receiving an array of bytes,

This is a [`ControlKeyboardMouse.au3`](https://ellysh.gitbooks.io/video-game-bots/content/Examples/ExtraTechniques/InputDeviceEmulation/ControlKeyboardMouse.au3) script, which demonstrates features of the `keyboard-mouse.ino` application:
```AutoIt
#include "CommInterface.au3"

func ShowError()
	MsgBox(16, "Error", "Error " & @error)
endfunc

func OpenPort()
	; This function is the same as one in the ControlKeyboard.au3 script
endfunc

func SendArduinoKeyboard($hPort, $modifier, $key)
	if $modifier == NULL then
		local $command[5] = [0xDC, 0x1, 0xFF, $key, 0xFF]
	else
		local $command[5] = [0xDC, 0x2, $modifier, $key, 0xFF]
	endif

	_CommAPI_TransmitString($hPort, StringFromASCIIArray($command, 0, UBound($command), 1))

	if @error then ShowError()
endfunc

func GetX($x)
	return (127 * $x / 1366)
endfunc

func GetY($y)
	return (127 * $y / 768)
endfunc

func SendArduinoMouse($hPort, $x, $y, $button)
	local $command[5] = [0xDC, 0x3, GetX($x), GetY($y), $button]

	_CommAPI_TransmitString($hPort, StringFromASCIIArray($command, 0, UBound($command), 1))

	if @error then ShowError()
endfunc

func ClosePort($hPort)
	_CommAPI_ClosePort($hPort)
	if @error then ShowError()
endfunc

$hPort = OpenPort()

$hWnd = WinGetHandle("[CLASS:MSPaintApp]")
WinActivate($hWnd)
Sleep(200)

SendArduinoMouse($hPort, 250, 300, 1)

Sleep(1000)

$hWnd = WinGetHandle("[CLASS:Notepad]")
WinActivate($hWnd)
Sleep(200)

SendArduinoKeyboard($hPort, Null, 0x54) ; T
SendArduinoKeyboard($hPort, Null, 0x65) ; e
SendArduinoKeyboard($hPort, Null, 0x73) ; s
SendArduinoKeyboard($hPort, Null, 0x74) ; t

Sleep(1000)

SendArduinoKeyboard($hPort, 0x82, 0xB3) ; Alt+Tab

ClosePort($hPort)
```
Here we use two separate functions to send commands to the Arduino board. The `SendArduinoKeyboard` function sends a command to simulate keystroke actions. Implementation of this function similar to the `SendArduino` one from the `ControlKeyboardCombo.au3` script. But there are differences in the command format. We add here the second byte with an action code and the fifth byte for padding a command length to the required size. Also we replace a modifier byte to 0xFF value if the modifier is not required.

The `SendArduinoMouse` function sends command to simulate a mouse click. We have add only the second byte with an action code to its `$command` array.

This is an algorithm to test the script:

1. Upload the `keyaboard-mouse.ino` application to your Arduino board.
2. Launch the Paint application.
3. Launch the Notepad application.
4. Launch the control script.

The script simulates three actions one after each other:

1. Mouse click in the Paint window.
2. Typing the "Test" string in the Notepad window.
3. Switch windows by the *Alt+Tab* keystroke.

There is a question, why we use 0xFF byte instead of 0x0 one as padding for keypress commands? There is a limitation, which is caused by the `StringFromASCIIArray` AutoIt function. If this function meets a zeroed byte in the input ASCII array, it processes this byte as end of the string. This means that our command should not contain zeroed bytes.

## Summary

We have considered techniques to emulate keyboard and mouse devices with Arduino board. This emulator is controlled by the AutoIt scripts. It is possible to combine the pixel analysis feature of AutoIt together with the device emulation approach to develop more effective bots.This kind of bots are able to avoid anti-clicker protection approaches, which are based on checking the keyboard state.
