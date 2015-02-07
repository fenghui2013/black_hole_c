from socket import *
from time import *

def main():
    s = create_connection(('127.0.0.1', 8000))
    send_count = 0
    recv_count = 0
    count = 0
    while True:
        send_count += 1024 
        s.send("a"*1024)
        recv_count += len(s.recv(24*1024))
        print "send_count: ", send_count, "recv_count: ", recv_count
        if count > 10000:
            break
        count += 1
    s.close()

if __name__ == "__main__":
    #main()
    for i in range(1000):
        main()
