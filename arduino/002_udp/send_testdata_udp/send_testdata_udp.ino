#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiUDP.h>
#include <M5StickCPlus.h>

// arp -aでIPアドレスをすべて取得
// ipconfigで自身のIPアドレスを取得

const char ssid[] = ""; //WiFIのSSIDを入力
const char pass[] = ""; // WiFiのパスワードを入力

WiFiMulti WiFiMulti;

WiFiUDP wifiUdp; 
const char *pc_addr = "";
const int pc_port = 50007; //送信先のポート
const int my_port = 50008;  //自身のポート


void setup() {
  int sum=0;
  M5.begin();
  M5.Lcd.setRotation(3);  //Rotate the screen.  旋转屏幕
  WiFiMulti.addAP(ssid, pass);  //Add wifi configuration information.  添加wifi配置信息
  M5.lcd.printf("Waiting connect to WiFi: %s ...",ssid); //Serial port output format string.  串口输出格式化字符串

  int cnt=0;
  while(WiFi.status() != WL_CONNECTED) {
    cnt++;
    delay(500);
    M5.Lcd.print(".");
    if(cnt%10==0) {
      WiFi.disconnect();
      WiFi.begin(ssid, pass);
      M5.Lcd.println("");
    }
    if(cnt>=30) {
      ESP.restart();
    }
  }
  
  while(WiFiMulti.run() != WL_CONNECTED) {  //If the connection to wifi is not established successfully.  如果没有与wifi成功建立连接
    M5.lcd.print(".");
    delay(1000);
    sum+=1;
    if(sum==8) M5.lcd.print("Conncet failed!");
  }
  M5.lcd.println("\nWiFi connected");
  M5.lcd.print("IP address: ");
  M5.lcd.println(WiFi.localIP()); //The serial port outputs the IP address of the M5StickC.  串口输出M5StickC的IP地址
  delay(500);
  
  int dsaf = wifiUdp.begin(my_port);
  M5.lcd.print(dsaf);
  delay(2000);
}

void loop(){
  for(int i=0;i<100;i++){
  // データ送信
  wifiUdp.beginPacket(pc_addr, pc_port);
  wifiUdp.write('t');
  wifiUdp.write('e');
  wifiUdp.write('s');
  wifiUdp.write('t');
  wifiUdp.endPacket();
  M5.Lcd.print('.');


  delay(2000); //2秒待つ
  }

  //終了コマンド送信
  wifiUdp.beginPacket(pc_addr, pc_port);
  wifiUdp.write('&');
  wifiUdp.endPacket();
  delay(2000);
}
