from socket import *

def connect_server():
    s = create_connection(('127.0.0.1', 8000))
    command = ""
    while True:
        command = raw_input("please input your command:")
        if command == "":
            continue
        if command == "quit":
            break
        s.send(command)
        print(s.recv(1024))
    s.close()

if __name__ == "__main__":
    connect_server()
