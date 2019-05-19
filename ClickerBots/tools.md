# Инструменты для разработки

Вы начинаете писать программу, чтобы решить какую-то проблему. Есть большая вероятность, что до вас с ней или подобной задачей уже кто-то сталкивался. Скорее всего, для её решения были разработаны специальные инструменты. Поэтому лучшее что вы можете сделать, перед тем как начать писать код, - это изучить существующие языки программирования, фреймворки и библиотеки. Если вам повезет, вы найдете несколько готовых решений, которые будет достаточно скомпоновать вместе, для получения нужной функциональности. При этом важно не зацикливаться на использовании хорошо знакомых вам инструментов. Скорее всего, с их помощью вы сможете написать практически любое приложение, но на это уйдёт намного больше усилий, чем при использовании более подходящих средств.

В этом разделе мы рассмотрим несколько инструментов, которые хорошо подходят для разработки кликеров. В этой книге мы будем пользоваться ими для написания тестовых примеров. Не исключено, что для своих проектов, вы сможете найти или купить более подходящие инструменты.

## Язык программирования

**AutoIt** (www.autoitscript.com) - один из самых популярных языков программирования для различных задач автоматизации приложений. У него много возможностей, которые ускоряют разработку:

* Пройстой для изучения синтаксис.

* Подробная доступная онлайн документация и поддержка сообщества на форумах.

* Хорошая интеграция с функциями ОС (**WinAPI**) и сторонними библиотеками.

* Встроеный редактор исходного кода.

AutoIt хорошо подходит для изучения программирования с нуля. Все примеры этой главы будут написаны на нём. В комментариях к ним мы рассмотрим WinAPI функции, вызываемые через AutoIt. Таким образом, вам будет несложно переписать эти примеры на любом другом языке программирования.

**AutoHotKey** (ahkscript.org) - еще один подходящий язык для написания кликеров. У него есть практически все возможности AutoIt. Основное различие этих языков в синтаксисе. Некоторые примеры этой главы будет проще и быстрее реализовать на AutoHotKey. Но этот язык немного более сложен в изучении.

## Библиотеки обработки изображений

-----

# Tools

In order to start with bot development, you should be familiar with basic software tools that will help you to create, modify, debug and enhance your bot. Moreover, if you want to use these tools at most, you need to have an idea about how they work. This section will give you a short overview of the main tools that we are going to use during the bot development.

## Programming Language

[**AutoIt**](https://www.autoitscript.com/site/autoit) is one of the most popular [**scripting programming languages**](https://en.wikipedia.org/wiki/Scripting_language) for writing clicker bots. It has a lot of features that facilitate the development of automation scripts:

1. Easy to learn syntax.

2. Detailed online documentation and large community-based support forums.
3. Smooth integration with [**WinAPI**](https://en.wikipedia.org/wiki/Windows_API) functions and third-party libraries.
4. Built-in source code editor.

AutoIt is an excellent tool to start with programming. If you already have some experience with another programming language like C++, C#, Python, etc, you can use this language to implement examples from this chapter. Relevant WinAPI functions that are used by AutoIt will be mentioned.

[**AutoHotKey**](http://ahkscript.org) is a second scripting programming language that can be recommended for starting with game bots development. It has most of the AutoIt features but the syntax of this language is more unique. Some things will be simpler to implement with AutoHotKey than with AutoIt. But AutoHotKey language may be slightly more difficult to learn.

There are a lot of examples and guides about the development of game bots with both AutoIt and AutoHotKey languages on the Internet. Thus, you are free to choose a tool that you prefer. We will use AutoIt language in this chapter.

## Image Processing Libraries

AutoIt itself has many powerful image analysis methods. But there are two third-party libraries that will be extremely helpful for our purposes:

1. The [**ImageSearch**](https://www.autoitscript.com/forum/topic/148005-imagesearch-usage-explanation) library allows you to search a specified image in the game window.

2. The [**FastFind**](https://www.autoitscript.com/forum/topic/126430-advanced-pixel-search-library/) library provides advanced methods for searching a specified pixel in the game window. You can specify the number of [**pixels**](https://en.wikipedia.org/wiki/Pixel) of a given color. One of the FastFind library functions, for example, finds the regions where pixels of a given color are located as much close to each other as possible. Also, this library allows you to find the nearest pixel of a given color to the given point.

## Image Analysis Tool

Possibility to check image parameters (like pixel color or pixel coordinates) is very helpful for a developer of clicker bots. It helps to debug a bot application and check if image processing algorithms work correctly.

There are plenty of tools that allow you to take the color of pixels from the screen and to get current coordinates of a mouse cursor. You can easily find these tools with Google. I use the [**ColorPix**](https://www.colorschemer.com/colorpix_info.php) application that performs debugging tasks perfectly.

## Source Code Editors

AutoIt language is distributed with the customized version of SciTE editor. It is a great editor for programming and debugging AutoIt scripts. But more universal editors like [**Notepad++**](https://notepad-plus-plus.org) are more suitable if you use another programming language like Python or AutoHotKey. [**Microsoft Visual Studio**](https://www.visualstudio.com/en-us/products/visual-studio-express-vs.aspx) is the best choice for developers who prefer C++ and C# languages.

## API Hooking

We will develop example applications using high-level AutoIt language. The language encapsulates calls of WinAPI functions in the simplified interface. But it is necessary to know which WinAPI functions have been actually used by the internals of AutoIt. This allows you to understand algorithms better. Moreover, when you know the exact WinAPI function which was used, you can interact with it directly using your favorite programming language.

There are a lot of tools that provide WinAPI calls [**hooking**](https://en.wikipedia.org/wiki/Hooking). I use freeware [**API Monitor v2**](http://www.rohitab.com/apimonitor) application. It allows you to filter all hooked calls, to gather information about the process, to decode input and output parameters called functions and to view process memory. A full list of features is available on the developers website.
