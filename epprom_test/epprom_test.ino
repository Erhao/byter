#include <EEPROM.h>

void clearEEPROM() {
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
  Serial.begin(115200);
//  clearEEPROM();

  EEPROM.begin(256);
  for (int addr = 0; addr < 256; addr ++) {
    int data = EEPROM.read(addr);
    Serial.print(data);
    Serial.print(" ");
    delay(2);
  }
  Serial.println("End read");
}

void loop() {}
