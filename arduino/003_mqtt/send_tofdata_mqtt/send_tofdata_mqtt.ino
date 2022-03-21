//MQTTサーバーに距離センサのデータをPublishするプログラム

#include "M5StickCPlus.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "tof_hat.h"

WiFiClient espClient;
PubSubClient client(espClient);

tof_hat tof_hat;

// 接続する無線LANのSSID(2.4GHz)と暗号化キー、MQTTサーバーのIPアドレス(orドメイン名)を入力。
const char* ssid = "";
const char* password = "";
const char* mqtt_server = "";

const char* topic_name = "/pub/M5StickCPlus";

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setupWifi();
void callback(char* topic, byte* payload, unsigned int length);
void reConnect();

void setup() {
  M5.begin();
  M5.Lcd.setRotation(3);
  setupWifi();
  client.setServer(mqtt_server, 1883);  //接続先のサーバーを設定
  client.setCallback(callback); //メッセージCallBack関数の設定
}

void loop() {
  if (!client.connected()) {
    reConnect();
  }
  client.loop();  //この関数は、クライアントが受信メッセージを処理し、サーバーとの接続を維持するために定期的に呼び出される

  uint16_t dist = tof_hat.measure_distance();
  snprintf (msg, MSG_BUFFER_SIZE, "%ld", dist);
  M5.Lcd.print("Publish message: ");
  M5.Lcd.print(msg);
  M5.Lcd.println(" mm");
  client.publish(topic_name, msg);
  ++value;
  if(value%14==0){
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0,0);
    value = 0;
  }
  delay(200);
}

void setupWifi() {
  delay(10);
  M5.Lcd.printf("Connecting to %s",ssid);
  WiFi.mode(WIFI_STA);  //WiFiステーションモードに設定する。 
  WiFi.begin(ssid, password); //wifi接続を開始

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
  }
  M5.Lcd.printf("\nSuccess\n");
}

void callback(char* topic, byte* payload, unsigned int length) {
  M5.Lcd.print("Message arrived [");
  M5.Lcd.print(topic);
  M5.Lcd.print("] ");
  for (int i = 0; i < length; i++) {
    M5.Lcd.print((char)payload[i]);
  }
  M5.Lcd.println();
}

void reConnect() {
  while (!client.connected()) {
    M5.Lcd.print("Attempting MQTT connection...");
    // ランダムなクライアントIDを作成
    String clientId = "M5Stack-";
    clientId += String(random(0xffff), HEX);
    // 接続試行
    if (client.connect(clientId.c_str())) {
      M5.Lcd.printf("\nSuccess\n");
      // 接続後、指定されたトピックにメッセージを送信する
      client.publish("M5Stack", "hello world");
      // ... メッセージを受信する
      client.subscribe("M5Stack");
    } else {
      M5.Lcd.print("failed, rc=");
      M5.Lcd.print(client.state());
      M5.Lcd.println("try again in 5 seconds");
      delay(5000);
    }
  }
}
