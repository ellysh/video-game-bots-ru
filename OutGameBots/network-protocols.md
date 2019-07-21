# Сетевые протоколы

В первой главе мы рассмотрели архитектуру типичной онлайн игры. Как вы помните, в ней игровой клиент взаимодействует с сервером через сеть (в большинстве случаев это Интернет). Для отправки пакетов клиент вызывает WinAPI функции. ОС обрабатывает эти вызовы и отправляет указанные данные по сети. Для этого используется сетевая плата, функции которой доступны ОС благодаря драйверу устройства. Возникает вопрос: как именно происходит передача игровых данных по сети? Исследуем этот вопрос.

## Задачи при передаче данных

Чтобы лучше понять существующие решения в какой-то технической области, будет разумным рассмотреть исходные задачи, которые к ним привели. Представим, что мы с вами разработчики ПО, и нам поставили задачу передать данные игрового клиента на сервер через сеть.

У нас есть два устройства, подключенных к сети, как на иллюстрации 4-1. Такие устройства называются **сетевыми хостами**.

![Игровой клиент и сервер](Figure_4-1.png)

**Иллюстрация 4-1.** *Игровой клиент и сервер, соединенные сетью*

Самый прямолинейный и простой способ решить поставленную задачу, - реализовать алгоритм передачи данных целиком в игровом клиенте. Этот алгоритм может выглядеть следующим образом:

1. Скопировать все состояния игровых объектов в байтовый массив. Такой массив называется [**сетевым пакетом**](https://ru.wikipedia.org/wiki/%D0%9F%D0%B0%D0%BA%D0%B5%D1%82_(%D1%81%D0%B5%D1%82%D0%B5%D0%B2%D1%8B%D0%B5_%D1%82%D0%B5%D1%85%D0%BD%D0%BE%D0%BB%D0%BE%D0%B3%D0%B8%D0%B8)).

2. Скопировать подготовленный пакет в память, доступную для сетевой платы. Обычно эта память доступна через [DMA](https://ru.wikipedia.org/wiki/%D0%9F%D1%80%D1%8F%D0%BC%D0%BE%D0%B9_%D0%B4%D0%BE%D1%81%D1%82%D1%83%D0%BF_%D0%BA_%D0%BF%D0%B0%D0%BC%D1%8F%D1%82%D0%B8).

3. Дать плате команду на отправку пакета.

Наш алгоритм будет успешно справляться с передачей данных до тех пор покав сети будет только два устройства. Но что произойдёт, если подключить третье устройство, как на иллюстрации 4-2?

![Сеть из трёх хостов](Figure_4-2.png)

**Иллюстрация 4-2.** *Сеть из трёх хостов*

В этому случае нам не обойтись без дополнительного устройства, известного как **сетевой коммутатор** (network switch). У обычной современной сетевой платы Ethernet есть только один порт. Она рассчитана на подключение точка-точка. Поэтому только сетевых плат недостаточно для подключения трёх устройств между собой. На данный момент будем рассматривать коммутатор, только как средство физического подключения нескольких хостов к одной сети.

С появлением третьего устройства в сети у нас появилась новая задача. Каким-то образом необходимо различать хосты и направлять игровые данные от клиента на сервер, но не на телевизор. Вы можете возразить, что нет серьёзной проблемы, если телевизор получит несколько ненужных ему пакетов. Он может их просто проигнорировать. Эта мысль верна до тех пор, пока у нас небольшая сеть. Но что случится, если в нашей сети сотни хостов? Если каждый узел будет посылать трафик для каждого, сеть окажется перегружена. Задержки в передаче пакетов станут настолько велеки, что никакого эффективного взаимодействия между хостами не получится. Причина этого в том, что сетевые кабели и платы имеют ограниченную пропускную способность в силу аппаратных особенностей. С этим ресурсом нам следует работать осмотрительно.

Проблему различия хостов в сети можно решить, если каждому из них назначить уникальный идентификатор. Мы пришли к первому решению, которое приняли настоящие разработчики сетей.  **MAC-адрес** - это уникальный идентификатор сетевой платы или другого передающего в сеть устройства. Этот адрес назначается изготовителем на этапе производства устройства. Он уникален и неизменен. Теперь наше игровое приложение на клиенте может добавлять MAC-адрес целевого хоста к каждому передаваемому пакету. Благодаря адресу сетевой коммутатор сможет перенаправлять пакет только на порт, к которому подключён целевой хост.

Откуда коммутатор знает MAC-адреса хостов подключённые к его портам? Для этого он следит за всеми входящими на каждый порт пакетами. Из них он читает MAC адрес отправителя и добавляет его в таблицу разрешения адресов, также известную как Address Resolution Logic (ARL). В этой таблице каждая строка содержит MAC-адрес и соответствующий ему порт.

Когда сервер получает пакет клиента, ему может потребоваться подтвердить корректность принятых данных либо запросить повторной передачи. Для этого нужно знать MAC-адрес отправителя. Поэтому будет разумным при отправке пакета клиентом добавлять не только MAC-адрес целевого хоста, но и свой собственный.


