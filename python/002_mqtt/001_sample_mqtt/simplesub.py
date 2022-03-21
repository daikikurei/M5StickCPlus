import paho.mqtt.subscribe as subscribe

topics = 'test'

def print_msg(client, userdata, message):
    print("%s : %s" % (message.topic, message.payload))

while True:
    subscribe.callback(print_msg, "test", hostname="localhost")