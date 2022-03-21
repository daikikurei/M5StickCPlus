from socket import * # from socket import socket, AF_INET, SOCK_DGRAM

def setup_udp_recive(port):
    HOST_NAME = ""   

    print("start network")
    addr = (HOST_NAME, port)

    print("network setup started")

    #udp_sockという名前のUDPソケット生成
    #ipv4を使うので、AF_INET
    #udp通信を使いたいので、SOCK_DGRAM
    udp_sock = socket(AF_INET, SOCK_DGRAM)
    udp_sock.settimeout(0.0001)
    print("Connected !!")
    print("Network info -->" + str(addr))

    #ブロードキャストするときは255.255.255.255と指定せずに空文字を指定してbind
    #udp_sockというソケットにaddressを紐づける
    udp_sock.bind(addr)

    print("setup finished")

    return udp_sock