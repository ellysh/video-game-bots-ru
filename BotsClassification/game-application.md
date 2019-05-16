# Игровое Приложение

Перед изучением внутреннего устройства ботов, нам следует рассмотреть работу типичного игрового приложения. Существует несколько различных игровых жанров, но все приложения имеют сходный принцип работы с точки зрения разработчиков.

Сначала мы рассмотрим онлайн игру. Иллюстрация 1-1 демонстрирует её логические элементы.

![Схема приложения онлайн игры](game-application.png)

**Иллюстрация 1-1.** *Элементы типичного приложения онлайн игры*

Запуская игру на компьютере, вы создаете новый **вычислительный процесс**. Он имеет собственную **область памяти**, которая выделяется **операционной системой** (ОС). Память - это только один из ресурсов предоставляемых ОС. Примеры других ресурсов - это устройства ввода-вывода (монитор, клавиатура, мышь, сетевой адаптер). Процессорное время - еще один ресурс, который определяет как часто игровой процесс получает управление в многозадачной ОС.

Возможно, вы спросите: "Зачем вообще нужна ОС для запуска игры? Разве не проще запускать приложение без неё?" ОС - это прежде всего удобная платформа для разработки приложений. Без неё, каждой компании по разработке ПО пришлось бы разрабатывать собственные средства для работы с устройствами. Это потребовало бы много времени и усилий. Намного проще использовать **драйвера устройств**, предоставляемые ОС.

Вернемся к иллюстрации 1-1. Прямоугольники соответствуют элементам приложения, а стрелки - направлению передачи данных.

ОС обрабатывает команды работающего процесса для отображения картинки на мониторе или для отправки сетевых пакетов на сервер через сетевой адаптер. Также ОС отвечает за уведомление процесса о событиях на устройствах. Примеры таких событий: нажатие клавиши на клавиатуре или получение пакета от сервера. Ос выполняет эти задачи с помощью драйверов устройств и **системных библиотек**. На иллюстрации они объединены в единый блок "Операционная Система".

Рассмотрим обработку однократного действия игрока. Воспользуемся, иллюстрацией для отслеживания элементов учавствующих в обработке. Предположим, что мы перемещаем персонажа. Для этого нажимаем клавишу на клавиатуре. Обработка нажатия состоит из следующих шагов:

1. **Устройство ввода -> Операционная система**
Клавиатура посылает сигнал о нажатии клавиши **контроллеру прерываний**. Это устройство передает сигналы в процессор в порядке очереди и с учётом приоритетов. На программном уровне сигналы обрабатываются драйвером ОС.

2. **Операционная система -> Клиент игрового приложения**
ОС получает от драйвера событие, соответствующее нажатию клавиши. Затем ОС сообщает процессу игры об этом событии. Обычно, события нажатий клавиш получает процесс, чье окно является активным в данный момент. Предположим, что активно игровое приложение.

3. **Клиент игрового приложения**
После получения события о нажатии клавиши, процесс обновляет состояние игровых объектов в своей памяти. В нашем случае изменение касается местоположения персонажа.

4. **Клиент игрового приложения -> Операционная система**
Процесс должен сообщить игровому серверу о новом местоположении персонажа. Для этого процесс обращается к ОС через системную библиотеку, чтобы отправить сетевой пакет. Этот пакет содержит новую информацию о персонаже. Системная библиотека обращается к драйверу сетевого адаптера, чтобы отправить пакет.

5. **Операционная система -> Игровой сервер**
Игровой сервер получает сетевой пакет. Затем он проверяет, соответствует ли новая информация о персонаже игровым правилам. Если проверка прошла успешно, сервер принимает эти данные и отправляет клиенту подтверждение. Если к серверу подключено несколько клиентов, он рассылает новую информацию о персонаже им всем.

6. **Операционная система -> Клиент игрового приложения**
Через контроллер прерываний сетевой адаптер посылает сигнал в процессор о получении сетевого пакета от игрового сервера. Сигнал обрабатывается драйвером, создается соответствующее событие на уровне ОС, которое передается процессу игрового приложения.

7. **Клиент игрового приложения**
Процесс извлекает из сетевого пакета код подтверждения игрового сервера. Если код сообщает об ошибке, местоположение игрового персонажа остается неизменным. В противном случае, процесс пометит в своей памяти, что новая информация о персонаже была успешно принята сервером.

8. **Клиент игрового приложения -> Операционная система**
Процесс игрового приложения обращается к системной библиотеке ОС для отображения на мониторе нового положения персонажа.

9. **Операционная система -> Устройство вывода**



----


The launched game client application is one of plenty [**computing processes**](https://en.wikipedia.org/wiki/Process_%28computing%29) of a [**operating system**](https://en.wikipedia.org/wiki/Operating_system) (OS). Each of these processes has a separate [**memory sandbox**](http://duartes.org/gustavo/blog/post/anatomy-of-a-program-in-memory) that has been allocated by OS. OS provides an access to devices for all launched processes. Examples of the devices are a monitor, keyboard, mouse, network adapter and etc. OS handles requests from the processes for data output events like displaying a picture on the screen or sending a packet through the network adapter. Also, OS notifies processes about data input events like keyboard key pressing or reception of a packet on the network adapter. OS performs all these tasks using [**drivers**](https://en.wikipedia.org/wiki/Device_driver) and [**system libraries**](https://en.wikipedia.org/wiki/Library_%28computing%29). They are combined in the block with "Operating System" name in the scheme for simplification purposes.

Now we will consider an algorithm for processing concrete player action. We will use the scheme to follow which components will participate in this processing. Let us suppose, that you want to move player's character. You press an appropriate arrow key on the keyboard to do it. This is an approximate list of the steps that will provide the character's movement action:

1. **Input Device => Operating System**<br/>
A keyboard driver signals OS by the [**interrupt**](https://en.wikipedia.org/wiki/Interrupt) mechanism that an arrow key has been pressed.
2. **Operating System => Game Client Application**<br/>
OS handles the keyboard driver notification. Then OS notifies a process about the keyboard event. Usually, this notification will be received by the process, whose window has an active state at the moment. Let us assume, that this is the game application's process.
3. **Game Client Application**<br/>
A game application's process receives the keyboard event notification from OS. The process updates the state of game objects in own memory according to a new character's position.
4. **Game Client Application => Operating System**<br/>
Game application's process requires OS to send a network packet to the game server via a network library of the OS. The packet contains information about the new character's position. The library uses a driver of a network adapter to send the packet.
5. **Operating System => Game Server**<br/>
The game server receives the network packet from the client host. Then it validates the new character's position according to game rules. If this validation is succeeded, the server sends a network packet to the client host. There is a confirmation of the new character's position in the packet.
6. **Operating System => Game Client Application**<br/>
OS notifies the game application's process about a packet receiving from the game server. The process reads data from the packet via a network library of the OS. The library uses the network adapter driver to read the received data.
6. **Game Client Application**<br/>
Game application's process extracts server's confirmation of the new character's position from the received network packet. If the confirmation does not exist, the character's position is kept unchanged. Otherwise, the new position will be assigned to the character object.
7. **Game Client Application => Operating System**<br/>
Game application's process requires OS to update a current picture at the screen according to the new character's position.
8. **Operating System => Output Device**<br/>
OS requires a graphic library like [**OpenGL**](https://en.wikipedia.org/wiki/OpenGL) or [**DirectX**](https://en.wikipedia.org/wiki/DirectX) to draw a new picture on the screen. Graphic library performs calculations for the new picture and draws it using a video driver.

That is all what is needed for moving player's character.

This algorithm is still almost the same for other player's actions. It may vary in case of internal game events. This kind of events usually happen on a game server side. Algorithm of  processing these events will contain the steps from 5 to 8 numbers of the considered algorithm. A game server notifies client that something has been changed. A game application's process updates the state of game objects and requires to refresh a screen picture.

The considered game application scheme is still valid for the most of modern popular on-line games. Game genre like RPG, real-time strategy, shooter, sports and etc is not important in this case. All of these genres use similar mechanisms and [**client-server architecture**](https://en.wikipedia.org/wiki/Client%E2%80%93server_model).

This is the corrected scheme for a game with the single play mode only:

![Local Game Application Scheme](game-local-application.png)

The game server component should be excluded from the scheme. All player actions and game events affect memory of a game application's process only. State of all game objects is  stored on local PC. Please note, that game objects' state is stored on both server-side and client-side in case of on-line games. But server-side information has a higher priority than a client-side one. Therefore, if the state of game objects differs on the server-side and the client-side, the server-side state is chosen as the original. Game server implicitly controls a correctness of game objects' state in this way. But nobody controls this correctness in case of single player game.

Single player and on-line games have the same interaction algorithm with the OS resources via drivers and system libraries.
