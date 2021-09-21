#include <EEPROM.h>


void clearCount() {
  EEPROM.begin(1024);
  // write a 0 to all 4 bytes of the EEPROM
  for (int i = 0; i < 1024; i++) {
    EEPROM.write(i, 0);
  }
  int res = EEPROM.commit();
  Serial.println("");
  Serial.println("res clearCount " + (String) res);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  clearCount();

  EEPROM.begin(256);
  EEPROM.write(0, 999);
  EEPROM.write(1, 10);
  EEPROM.write(2, 255);
  EEPROM.write(3, 256);
  EEPROM.write(4, 257);
  EEPROM.commit();
  Serial.println("");
  Serial.println("End write");

  EEPROM.begin(256);
  for (int addr = 0; addr < 256; addr ++) {
    int data = EEPROM.read(addr);
    Serial.print(data);
    Serial.print(" ");
    delay(2);
  }
  Serial.println("End read");
}

void loop() {
  // put your main code here, to run repeatedly:

}
