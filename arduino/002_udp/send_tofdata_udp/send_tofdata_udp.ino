//Wifi
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiUDP.h>

//M5StickCPlus
#include <M5StickCPlus.h>

//ToF
#include <Wire.h>
#define VL53L0X_REG_IDENTIFICATION_MODEL_ID         0xc0
#define VL53L0X_REG_IDENTIFICATION_REVISION_ID      0xc2
#define VL53L0X_REG_PRE_RANGE_CONFIG_VCSEL_PERIOD   0x50
#define VL53L0X_REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD 0x70
#define VL53L0X_REG_SYSRANGE_START                  0x00
#define VL53L0X_REG_RESULT_INTERRUPT_STATUS         0x13
#define VL53L0X_REG_RESULT_RANGE_STATUS             0x14
#define ToF_ADDR 0x29//the iic address of tof

// arp -aでIPアドレスをすべて取得
// ipconfigで自身のIPアドレスを取得

const char ssid[] = ""; //WiFIのSSIDを入力
const char pass[] = ""; // WiFiのパスワードを入力

WiFiMulti WiFiMulti;

WiFiUDP wifiUdp; 
const char *pc_addr = "";  //"";
const int pc_port = 50007; //送信先のポート
const int my_port = 50008;  //自身のポート

byte gbuf[16];

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

  // put your setup code here, to run once:
  Wire.begin(0, 26, 400000);// join i2c bus (address optional for master)
  Serial.begin(115200);  // start serial for output
  Serial.println("test started.");

  //---osmar
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
}

void loop(){
//  Serial.println("----- START TEST ----");
//  measure_distance();
//  Serial.println("----- END TEST ----");
//  Serial.println("");
//  delay(100);
  Serial.println("----- START TEST ----");
  uint16_t dist = measure_distance();
  Serial.println("----- END TEST ----");
  Serial.println("");
  
//  int size = sizeof(dist) / sizeof(int);
//  for(int i=0; i < size; i++){
//    wifiUDP.Write(dist[i]);
//  }

  String d = String(dist);
  int size = d.length();
  
  // データ送信
  wifiUdp.beginPacket(pc_addr, pc_port);
  for(int i=0; i < size; i++){
    wifiUdp.write(d[i]);
  }
  
  wifiUdp.endPacket();
  
  delay(200); //1秒待つ
}

uint16_t measure_distance() {
  write_byte_data_at(VL53L0X_REG_SYSRANGE_START, 0x01);

  read_block_data_at(VL53L0X_REG_RESULT_RANGE_STATUS, 12);//read 12 bytes once

  uint16_t dist = makeuint16(gbuf[11], gbuf[10]);//split distance data to "dist"
  byte DeviceRangeStatusInternal = ((gbuf[0] & 0x78) >> 3);

  Serial.print("distance ");  Serial.println(dist);

  M5.Lcd.setCursor(50, 80);
  M5.Lcd.fillRect(50, 80, 60, 25, BLACK);
  M5.Lcd.print(dist);

  return dist;
}

uint16_t bswap(byte b[]) {
  // Big Endian unsigned short to little endian unsigned short
  uint16_t val = ((b[0] << 8) & b[1]);
  return val;
}

uint16_t makeuint16(int lsb, int msb) {
    return ((msb & 0xFF) << 8) | (lsb & 0xFF);
}

uint16_t VL53L0X_decode_vcsel_period(short vcsel_period_reg) {
  // Converts the encoded VCSEL period register value into the real
  // period in PLL clocks
  uint16_t vcsel_period_pclks = (vcsel_period_reg + 1) << 1;
  return vcsel_period_pclks;
}

/*
 * IIC Functions
 */
/* function description: write one byte data */
void write_byte_data(byte data) {
  Wire.beginTransmission(ToF_ADDR);
  Wire.write(data);
  Wire.endTransmission();
}

/* function description: write one byte data to specifical register */
void write_byte_data_at(byte reg, byte data) {
  Wire.beginTransmission(ToF_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

/* function description: read two bytes data to specifical register */
void write_word_data_at(byte reg, uint16_t data) {
  byte b0 = (data &0xFF);
  byte b1 = ((data >> 8) && 0xFF);

  Wire.beginTransmission(ToF_ADDR);
  Wire.write(reg);
  Wire.write(b0);
  Wire.write(b1);
  Wire.endTransmission();
}

/* function description: read one byte data */
byte read_byte_data() {
  Wire.requestFrom(ToF_ADDR, 1);
  while (Wire.available() < 1) delay(1);
  byte b = Wire.read();
  return b;
}

/* function description: read one byte data from specifical register */
byte read_byte_data_at(byte reg) {
  //write_byte_data((byte)0x00);
  write_byte_data(reg);
  Wire.requestFrom(ToF_ADDR, 1);
  while (Wire.available() < 1) delay(1);
  byte b = Wire.read();
  return b;
}

/* function description: read two bytes data from specifical register */
uint16_t read_word_data_at(byte reg) {
  write_byte_data(reg);
  Wire.requestFrom(ToF_ADDR, 2);
  while (Wire.available() < 2) delay(1);
  gbuf[0] = Wire.read();
  gbuf[1] = Wire.read();
  return bswap(gbuf);
}

/* function description: read multiple bytes data from specifical register */
void read_block_data_at(byte reg, int sz) {
  int i = 0;
  write_byte_data(reg);
  Wire.requestFrom(ToF_ADDR, sz);
  for (i=0; i<sz; i++) {
    while (Wire.available() < 1) delay(1);
    gbuf[i] = Wire.read();
  }
}
