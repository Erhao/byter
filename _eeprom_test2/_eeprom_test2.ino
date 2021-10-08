#include <EEPROM.h>

unsigned long l;

int s = sizeof(int);
int sl = sizeof(l);

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

  Serial.println("sizeof int:");
  Serial.println(s);

  Serial.println("sizeof unsigned long:");
  Serial.println(sl);

  EEPROM.put(1, 510);
  int res = EEPROM.commit();
  delay(10);
  Serial.println("write <v1> to addr 1 ");
  Serial.println(res);

  l = 4294967294;
  EEPROM.put(20, 2147483647);
  int res2 = EEPROM.commit();
  delay(10);
  Serial.println("write <v2> to addr 20");
  Serial.println(res2);

  unsigned int ui = 2147483649;
  EEPROM.put(30, ui);
  int res3 = EEPROM.commit();
  delay(10);
  Serial.println("write <v3> to addr 30");
  Serial.println(res3);

  EEPROM.begin(256);
  for (int addr = 0; addr < 256; addr ++) {
    int data = EEPROM.read(addr);
    Serial.print(data);
    Serial.print(" ");
    delay(2);
  }
  Serial.println("End read");
  delay(10);

  int r_val1, r_val2;
  unsigned int r_val3;

  EEPROM.get(1, r_val1);
  Serial.println("get from addr 1");
  Serial.println(r_val1);

  EEPROM.get(20, r_val2);
  Serial.println("get from addr 20");
  Serial.println(r_val2);

  EEPROM.get(30, r_val3);
  Serial.println("get from addr 30");
  Serial.println(r_val3);

  Serial.println("unsigned long");
  Serial.println(l);
}

void loop() {
  // put your main code here, to run repeatedly:

}
