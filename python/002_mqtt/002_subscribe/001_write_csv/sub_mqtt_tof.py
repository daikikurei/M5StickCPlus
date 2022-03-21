import paho.mqtt.client as mqtt
import csv
from my_module import mqtt_func as mf

# host = 'localhost'
host = ''
port = 1883
topic = '/pub/M5StickCPlus'
csv_file_path = './csv/output.csv'
csv_output_count = 100

if __name__ == '__main__':

    mqtt = mf.MqttFunc(host, port, topic, csv_file_path, csv_output_count)
    mqtt.connect()