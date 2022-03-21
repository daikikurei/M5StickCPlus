import paho.mqtt.client as mqtt
import csv
import datetime
import os
from operator import itemgetter

class MqttFunc:

    def __init__(self, host, port, topic, csv_folder_path, csv_output_count, csv_max_storages):
        self.host = host
        self.port = port
        self.topic = topic

        self.client = mqtt.Client(protocol=mqtt.MQTTv311)
        self.client.on_connect = self.on_connect
        self.client.on_disconnect = self.on_disconnect
        self.client.on_message = self.on_message

        self.csv_folder_path = csv_folder_path
        
        #self.csv_folder_pathが存在しなければ新規作成
        if not os.path.exists(self.csv_folder_path):
            os.mkdir(self.csv_folder_path)
            print("create_new_folder:" + self.csv_folder_path)

        #csv出力タイミング用カウント
        self.count = 0
        #self.countがこの値になったらcsv出力を行う。
        self.csv_output_count = csv_output_count
        #self.csv_folder_path内のcsvファイルがこの値を超えたら、古いcsvを削除する。
        self.csv_max_storages = csv_max_storages
        #データ格納用配列
        self.sub_data = []

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
        if self.count >= self.csv_output_count:
            #csv出力
            self.out_put_csv()
            self.count = 0
            self.sub_data = []
        
    #MQTT待ち受けループ用
    def connect(self):
        self.client.connect(self.host, port=self.port, keepalive=60)

        # 待ち受け状態にする
        self.client.loop_forever()
    
    #csv出力用
    def out_put_csv(self, ):
        #現在日時を取得
        dt_now = datetime.datetime.now()
        csv_file_path = self.csv_folder_path + '/' + 'output_' + dt_now.isoformat() +'.csv'

        #csv出力
        with open(csv_file_path, 'w') as f:
            for index, item in enumerate(self.sub_data):
                print(self.sub_data[index], file=f)
        
        #csv削除
        self.delete_csv()
    
    #csv削除用            
    def delete_csv(self):
        filelists = []
        
        #フォルダ内のファイルについてループ
        for file in os.listdir(self.csv_folder_path):
            #pathをbase（拡張子以外の部分）とext（ピリオドを含む拡張子）に分割
            base, ext = os.path.splitext(file)
            #.csv拡張子だけリストに追加
            if ext == '.csv':
                filelists.append([self.csv_folder_path + '/' + file, os.path.getctime(self.csv_folder_path + '/' + file)]) #[file名, 作成日時のエポック秒]
        
        #作成日時の昇順でソート        
        filelists.sort(key=itemgetter(1), reverse=True)
        
        #ファイルを削除
        for i,file in enumerate(filelists):
            if i > self.csv_max_storages - 1:
                os.remove(file[0])
        

            