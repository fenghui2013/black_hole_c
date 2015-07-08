from socket import *
from time import *

def connect_server():
    http_message = "GET /index.html HTTP/1.1\r\nHOST: 127.0.0.1\r\n\r\n"
    s = create_connection(('127.0.0.1', 8000))
    s.send(http_message)
    print(s.recv(1024))
    sleep(1)
    s.close()

if __name__ == "__main__":
    connect_server()
