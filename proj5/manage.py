import socket
import sys

def main():
    host = '127.0.0.1'
    port = 5666
    addr = (host, port)
    backlog = 10 # Num of connections
    size = 4096 # Maximum data stream size

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(addr)
    s.listen(backlog)

    while True:
        client, address = s.accept()
        data = client.recv(size)
        if data:
            print data
        client.close()

if __name__ == "__main__":
    main()
