import paho.mqtt.client as mqtt
import csv

class MqttFunc:

    def __init__(self, host, port, topic, csv_file_path, csv_output_count):
        self.host = host
        self.port = port
        self.topic = topic

        self.client = mqtt.Client(protocol=mqtt.MQTTv311)
        self.client.on_connect = self.on_connect
        self.client.on_disconnect = self.on_disconnect
        self.client.on_message = self.on_message

        self.csv_file_path = csv_file_path

        #csv出力タイミング用カウント
        self.count = 0
        #self.countがこの値になったらcsv出力を行う。
        self.csv_output_count = csv_output_count
        #データ格納用配列
        self.sub_data = []

    def test_method1(self):
        print("test_method2:" + str(self.val))

    #Brokerと接続
    def on_connect(self, client, userdata, flags, respons_code):
        print('status {0}'.format(respons_code))

        client.subscribe(self.topic)

    #Brokerと切断
    def on_disconnect(self, client, userdata, flag, rc):
        if  rc != 0:
            print("disconnect broker")

    #メッセージ受信
    def on_message(self, client, userdata, msg):
        message = str(msg.payload)
        message = message.lstrip("b'")
        message = message.rstrip("'")
        print(msg.topic + ' ' + message)
        self.sub_data.append(message)
        self.count += 1

        #csv出力
        if self.count >= self.csv_output_count:
            with open(self.csv_file_path, 'a') as f:
                for index, item in enumerate(self.sub_data):
                    print(self.sub_data[index], file=f)

            self.count = 0
            self.sub_data = []

    
    def connect(self):
        self.client.connect(self.host, port=self.port, keepalive=60)

        # 待ち受け状態にする
        self.client.loop_forever()