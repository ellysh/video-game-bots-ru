  while(1):
    # получить от сервера ход игрока
    s.recv(1024, socket.MSG_WAITALL)
    data = s.recv(1024, socket.MSG_WAITALL)
    print(data)

    start_num = 7 - data[2]
    end_num = 7 - data[4]

    # отправить ход бота
    s.send(bytes([0x07, 0, 0, 0]))
    s.send(bytes([0, 0, start_num, data[3], end_num, data[5], 0x00]))
