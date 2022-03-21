import sys
from socket import *
import time
from my_module import setup_udp_recive as sur

"""
IPアドレスの確認方法
コマンドプロンプトで「ipconfig」と入力し
と入力し、
［IPv4 アドレス］または［IPアドレス］の値が、使用しているパソコンのIPアドレスです。
"""

port = 50007

if __name__ == "__main__":
    udp_sock = sur.setup_udp_recive(port)
    while True:
        try:
            (data, addr) = udp_sock.recvfrom(1024)
        except timeout:
            continue

        if addr !=0:
            str_data = data.decode('utf-8')
            print(f"get message from {addr} --> {str_data}")