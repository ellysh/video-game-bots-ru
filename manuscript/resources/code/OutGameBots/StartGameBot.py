import socket

def main():
  s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
  s.settimeout(60)
  s.connect(("127.0.0.1", 55555))

  # получить от сервера подтверждение соединения
  s.recv(1024, socket.MSG_WAITALL)
  s.recv(1024, socket.MSG_WAITALL)

  # отправить имя пользователя на стороне клиента
  s.send(bytes([0x09, 0, 0, 0]))
  s.send(bytes([0x0C, 0x63, 0x6C, 0x5F, 0x75, 0x73, 0x65, 0x72, 0x00]))

  # получить от сервера уведомление о включении режима "Manual Edit"
  s.recv(1024, socket.MSG_WAITALL)
  s.recv(1024, socket.MSG_WAITALL)

  # отправить подтверждение клиентом режима "Manual Edit"
  s.send(bytes([0x01, 0, 0, 0]))
  s.send(bytes([0x17]))

  # получить от сервера уведомление о включении игровых часов
  s.recv(1024, socket.MSG_WAITALL)
  s.recv(1024, socket.MSG_WAITALL)

  s.close()

if __name__ == '__main__':
  main()
