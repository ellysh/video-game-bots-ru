# Example with Lineage 2

## Lineage 2 Overview

Now let's make a simple clicker bot for the popular MMORPG game Lineage 2. This helps us to apply knowledge in practice and approaches that have been already acquired. The gameplay of Lineage 2 is a very typical for RPG genre. A player should select one of the available characters before starting to play. Then you should do quests and hunt monsters to achieve new skills, extract resources and buy new items. The player is able to communicate and to cooperate with other players during the game. Other players are able to assist you in your activity or hamper you in achieving your goals. This feature encourages you to develop your character faster that helps you to resist the interference of other players. You will be able to participate in "team vs team" battles when you achieve a high level of your character. These PvP events are a main attraction of the game.

The most straightforward way to improve your character is hunting monsters. You will get experience points to improve your skills, gold to buy new items and random resources after killing a monster. We will focus on automation of this process because it allows you to develop a player's character in a comprehensive manner. Also, there are other ways to develop a character like trading, fishing, crafting new items and completing quests.

This is a screenshot of the Lineage 2 game:

![Lineage 2 Interface](lineage-interface.png)

This is a list of important interface elements on this screenshoot:
1. **Status Window** with current parameters of the player's character. The most important parameters are health points (HP) and mana points (MP).
2. **Target Window** with information of selected monster. Here you can see HP of the monster that you are attacking now.
3. **Shortcut Panel** with icons of available actions and skills that are attached to hotkeys.
4. **Chat Window** for input game commands and chatting with other players.

Understanding game interface allows us to make a clicker bot that will interact with the game in a more efficient manner. Detailed information about game interface available on the [wiki page](https://l2wiki.com/Game_Interface).

There are a lot of Lineage 2 servers. They differ by game version, extra gameplay features, and protection systems, which are used to prevent usage of bots. The most reliable and effective protection system is used on [official servers](http://www.lineage2.eu). But there are many different private servers apart from official one. We will use the [Rpg-Club](http://www.rpg-club.com) server in our example because the protection system on this server does not block clicker bots.

## Bot Implementation

This is a simplified algorithm of hunting monsters:
1. Select a monster by left button clicking on him. Another way to select a monster is typing a command in the chat window or use the macro with this command:
```
/target MonsterName
```
The full list of the game commands and manual for using macros are available [here](http://www.lineage2.com/en/game/getting-started/how-to-play/macros-and-commands.php).
2. Click the "attack" button on the Shortcut Panel. Another way to start attacking is pressing the *F1* (by default) keyboard key.
3. Wait until your character kills the monster.
4. Click the "pickup" button on the Shortcut Panel to pick up the items that have been dropped from the killed monster. You can also use a keyboard hotkey for this.

You can see that the algorithm is quite simple and easy to automate at first look.

### Blind Bot

First, we will implement the simplest type of bot. This bot will simply follow the steps mentioned above. However, it will not analyze a result of the performed actions. The bot will use keystroke simulation approach for performing these actions.

It will be helpful to consider a configuration of our Shortcut Panel before we start to write a code. This is a screenshot of the panel:

![Shortcut Panel](lineage-hotbar.png)

This is a list of actions and corresponding hotkeys on this panel:

| Hotkey | Command |
| -- | -- |
| *F1* | Attack the currently selected monster |
| *F2* | Use an offensive skill on the selected monster |
| *F5* | Use a health potion for restoring player's HP |
| *F8* | Pickup items near the player | 
| *F9* | Macro with `/target MonsterName` command to select a monster |
| *F10* | Select the nearest monster |

Now it becomes simple to associate hotkeys with algorithm steps and write a code. This is a script with [`BlindBot.au3`](https://ellysh.gitbooks.io/video-game-bots/content/Examples/ClickerBots/Lineage2Example/BlindBot.au3) name that implements all steps of our algorithm:
```AutoIt
#RequireAdmin

Sleep(2000)

while True
    Send("{F9}")
    Sleep(200)
    Send("{F1}")
    Sleep(5000)
    Send("{F8}")
    Sleep(1000)
wend
```
The first line of the script is a  [`#RequireAdmin`](https://www.autoitscript.com/autoit3/docs/keywords/RequireAdmin.htm) keyword. This keyword permits interaction between the script and an application that has been launched with administrator privileges. The Lineage 2 client can request the administrator privileges for launching. Next action in the script is two seconds delay, which is needed for you to switch to the Lineage 2 window. Now the bot is able to work in the active game window only. All bot actions are performed in the infinite `while` loop. This is a sequence of these actions:

1. `Send("{F9}")` - select a monster by the macro that is available via the *F9* key.
2. `Sleep(200)` - sleep 200 milliseconds. This delay is needed for the game application to select a monster and to draw a Target Window. You should remember that all actions in the game window take nonzero time. Often this time is much less than a time of human reaction and therefore it looks instantly.
3. `Send("{F1}")` - attack the selected monster.
4. `Sleep(5000)` - sleep 5 seconds while the character reaches a monster and kills it.
5. `Send("{F8}")` - pickup one item.
6. `Sleep(1000)` - sleeps 1 second while the character is picking up the item.

In this example, the action sequence of a bot is strictly defined. This means that every action will be successfully performed only if the previous action was successful. At first, the monster should be selected successfully, otherwise, all further actions will have no effect. Then, your character should reach and kill the selected monster within 5 seconds delay. Because sometimes it can take 1 second, and sometimes - more than 6 seconds depending on the distance between your character and selected monster. And the last, only one item should be dropped upon monster defeat. However, it is not always the case. To conclude, the script will work perfectly only if all the described conditions are fulfilled.

You can launch the bot script and test it. You will definitely see that your bot works correctly from time to time because very often the sequence is broken. However, it is not a big problem for this bot example with Lineage 2, because in general, it will continue working anyway. The features of in-game `/target` command and the attacking mechanism makes script sustainable. If the macro with the `/target` command is pressed twice, the same monster is selected. Thus, the bot will continue to attack the same monster until he is alive. If the monster is not killed during the first iteration of the loop, this process is continued on the next iteration. Moreover, the attack is not being interrupted even if pickup command is sent. This means that the character does not stop attacking the monster after exceeding the 5 seconds timeout for this action. There is still a problem with picking up monster drop (the bot picks up only 1 loot), but this issue can be solved by hardcoding an exact count of the items that are usually dropped by this type of monsters.

We can improve the script by moving each step of the algorithm to a separate function with a descriptive name. This makes the code more comprehensible. This is a [`BlindBotFunc.au3`](https://ellysh.gitbooks.io/video-game-bots/content/Examples/ClickerBots/Lineage2Example/BlindBotFunc.au3) script, which is separated into functions:
```AutoIt
#RequireAdmin

Sleep(2000)

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

while True
    SelectTarget()
    Attack()
    Pickup()
wend
```

### Adding Analysis

Let's improve our blind bot and make it more "clever". If the bot is able to check results of own actions, it will do much fewer mistakes. We will use pixels analysis approach to check the state of surrounding objects. But before we start to implement this feature, it will be helpful to add a mechanism of printing log messages. This mechanism will help us to trace bot's decisions and detect possible bugs.

This is a code snippet with a `LogWrite` function that prints a log message into the file:
```AutoIt
global const $LogFile = "debug.log"

func LogWrite($data)
    FileWrite($LogFile, $data & chr(10))
endfunc

LogWrite("Hello world!")
```
After execution of this code, you will get a file with the `debug.log` name which contains the "Hello world!" string. The `LogWrite` function is a wrapper for AutoIt [`FileWrite`](https://www.autoitscript.com/autoit3/docs/functions/FileWrite.htm) function. You can change a name and a path of the output file by changing a value of the `LogFile` constant.

The first assumption of the blind bot is success select a monster after usage a macro. When a monster is selected, the Target Window appears. We can search this window with functions from the FastFind library. If the window is present, the monster is selected successfully.

The `FFBestSpot` function provides suitable algorithm to solve this task. Now we should pick a color that is specific to the Target Window. Presence of this color on the screen will signal us that the Target Window is present. We can pick a color of monster's HP bar for example. This is a code snippet of the `IsTargetExist` function that checks a presence of the Target Window on the screen:
```AutoIt
func IsTargetExist()
    const $SizeSearch = 80
    const $MinNbPixel = 3
    const $OptNbPixel = 10
    const $PosX = 688
    const $PosY = 67
    
    $coords = FFBestSpot($SizeSearch, $MinNbPixel, $OptNbPixel, $PosX, $PosY, _
                         0x871D18, 10)

    const $MaxX = 800
    const $MinX = 575
    const $MaxY = 100
    
    if not @error then
        if $MinX < $coords[0] and $coords[0] < $MaxX and $coords[1] < $MaxY then
            LogWrite("IsTargetExist() - Success, coords = " & $coords[0] & _ 
                     ", " & $coords[1] & " pixels = " & $coords[2])
            return True
        else
            LogWrite("IsTargetExist() - Fail #1")
            return False
        endif
    else
        LogWrite("IsTargetExist() - Fail #2")
        return False
    endif
endfunc
```
The `PosX` and `PosY` coordinates define an approximate position of monster's HP bar. The `0x871D18` parameter matches to a red color of a full HP bar. This color is used by a searching algorithm. The `FFBestSpot` function searches pixels with the specified color in any position on the screen. Therefore, this function can detect player's HP bar instead the monster's HP bar. It happens when Target Window is not present. To avoid this mistake we check resulting coordinates provided by the `FFBestSpot` function. We compare resulting X coordinate (`coords[0]`) with maximum (`MaxX`) and minimum (`MinX`) allowed values. Also, the same comparison of Y coordinate (`coords[0]`) with maximum (`MaxY`) values is performed. The values of all coordinates depend on a screen resolution and position of the game window. You should adapt these coordinates to your screen configuration. 

We call the `LogWrite` function here to trace each conclusion of the `IsTargetExist` function. This helps us to check the correctness of the specified coordinates and the color value.

We can use new `IsTargetExist` function in both `SelectTarget` and `Attack` functions. This function checks if `SelectTarget` function is done successfully. Like this we ensure that a monster exists, so the bot can move to `Attack` function. In addition, the `IsTargetExist` function also checks if the monster is still alive during the `Attack` stage. If the bot executes the `Attack` functions, this means that a target monster is already selected. Finally, if the `IsTargetExist` function returns the `False` value, it means that the full HP bar does not present in the Target Window anymore. Thus, we can conclude that monster's HP bar is empty and a monster has died and out bot goes to the next looting stage.

This is the complete script with [`AnalysisBot.au3`](https://ellysh.gitbooks.io/video-game-bots/content/Examples/ClickerBots/Lineage2Example/AnalysisBot.au3) name, which checks the Target Window presence:
```AutoIt
#include "FastFind.au3"

#RequireAdmin

Sleep(2000)

global const $LogFile = "debug.log"
    
func LogWrite($data)
    FileWrite($LogFile, $data & chr(10))
endfunc

func IsTargetExist()
    ; SEE ABOVE
endfunc

func SelectTarget()
    LogWrite("SelectTarget()")
    while not IsTargetExist()
        Send("{F9}")
        Sleep(200)
    wend
endfunc

func Attack()
    LogWrite("Attack()")
    while IsTargetExist()
        Send("{F1}")
        Sleep(1000)
    wend
endfunc

func Pickup()
    Send("{F8}")
    Sleep(1000)
endfunc

while True
    SelectTarget()
    Attack()
    Pickup()
wend
```
Pay attention to a new implementation of `SelectTarget` and `Attack` functions. Command to select a monster is sent inside `SelectTarget` function until `IsTargetExist` function does not return "true". Similarly, the attack command is sent inside `Attack` function until the target monster had died. Also, log messages are printed in both functions. This allows us to distinguish a source of each call of the `IsTargetExist` function. Now the bot performs right actions according to the current game situation.

### Further Improvements

Now our bot is able to analyze the results of own actions. But there are several game situations that can lead to the character's death. The first problem is an existence of the aggressive monsters. Bot selects a monster with the specified name but all other monsters are ignored. The issue can be solved by sending a command to select the nearest monster. This command is available via the *F10* key in our Shortcut Panel.

This is a new `SelectTarget` function with selection of the nearest monster:
```AutoIt
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
```
Now the bot tries to select the nearest monster first. Then a macro with the `/target` command is used in case there is no monster near the character. This approach should solve the issue with the "invisible" aggressive monsters. 

The second problem is, there are obstacles in a hunting area. The bot can be stuck while moving to the selected monster. The simplest solution to this problem is to add a timeout for the attack action. If the timeout is exceeded, the bot moves randomly to avoid an obstacle.

This is a new version of the `Attack` and the `Move` functions, which provide the feature to avoid obstacles:
```AutoIt
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
```
We have added the `timeout` counter to the `Attack` function. This counter is incremented with each iteration of the `while` loop. Then this counter is compared with the threshold value of the `TimeoutMax` constant. When the `timeout` value reaches the threshold, bot recognizes this as "stuck on the obstacle" case and calls the `Move` function. This function performs a mouse click using the `MouseClick` function at random point.  The [`SRandom`](https://www.autoitscript.com/autoit3/docs/functions/SRandom.htm) AutoIt function is called here to initialize a random number generator. After that, the [`Random`](https://www.autoitscript.com/autoit3/docs/functions/Random.htm) function is called to generate random coordinates. The result of the `Random` function is between two numbers that passed as input parameters.

Another extra feature is added to the `Attack` function - usage of attack skill, which is available via the *F2* key. This allows the bot to kill monsters much more effectively.

Now our example bot is able to work autonomously for a long period of time. It can deal with obstacles and attack aggressive monsters. But there is still one thing to add in order to make the bot hardier. That is using health potions, which are bound to the *F5* key. To do that we need to analyze a level of character's HP bar in the Status Window. The algorithm of pixels analysis, which is similar to the `IsTargetExist` function, should solve this task.

## Summary

We have implemented an example of bot for Lineage 2 game. This is a typical clicker bot, which uses the most widespread approaches. Therefore, we can evaluate the effectiveness of our bot to make an overview of clicker type of bots at all.

This is a list of advantages of clicker bots:

1. Easy to develop, extend functionality and debug.
2. Easy to integrate with any version of the target game even there are significant differences in user interface between these versions.
3. It is difficult to protect a game against this type of bots.

This is a list of disadvantages of clicker bots:

1. The configuration of pixels' coordinates and colors is unique for each user.
2. In some unhandled cases (death, disconnect, NPC dialog, etc) the bot will not be able to continue working. 
3. Delays and timeouts lead to waste of time.
4. Analysis of bot actions has unreliable results. It means that the bot will make wrong actions in some cases.

Clicker bot can be effective for solving strictly defined tasks. These tasks should be easy to split into separate steps and algorithmize. Also, clicker bot works more reliable in case the algorithm has a minimal count of conditions, and the cost of a mistake is not extremely expensive.
