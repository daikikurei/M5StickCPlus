#include <M5StickCPlus.h>

void setup() {
  M5.begin();
  M5.Lcd.setCursor(0, 0, 2); 
  M5.Lcd.println("Serial sample.");
  Serial.begin(9600);
}

void loop() {
  if(Serial.available()) {
    String msg = Serial.readString();
    M5.Lcd.print("Received:" + msg);
    Serial.print(msg);
  }
}
