from socket import *
from time import *
import thread

def connect_server(a, b):
    s = create_connection(('127.0.0.1', 8000))
    send_count = 0
    recv_count = 0
    count = 0
    while True:
        send_count += 1024 
        s.send("a"*1024)
        recv_count += len(s.recv(24*1024))
        print "send_count: ", send_count, "recv_count: ", recv_count
        if count >= 10000:
            break
        count += 1
        print a, count
        sleep(0.5)
    print a, "close"
    s.close()

def main():
    for i in range(1000):
        try:
            thread.start_new_thread(connect_server, (i, 2))
        except:
            print "Error: unable to start thread"
        sleep(0.1)


if __name__ == "__main__":
    for i in range(1000):
        main()
    while True:
        sleep(1)
