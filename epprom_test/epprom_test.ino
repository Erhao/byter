#include <EEPROM.h>

int a = 0;
int addr = 0;
int value;

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

void setup()
{
  Serial.begin(115200);
  
  EEPROM.begin(4096); //申请操作4096字节数据
  for(addr = 0; addr<4096; addr++)
  {
    int data = addr;
    EEPROM.write(addr, addr); //写数据
  }
  EEPROM.commit(); //保存更改的数据

  Serial.println("End write");


  EEPROM.begin(4096); //申请操作4096字节数据
  for(addr = 0; addr<4096; addr++)
  {
    int data = EEPROM.read(addr); //读数据
    Serial.print(data);
    Serial.print(" ");
    delay(2);
    if((addr+1)%256 == 0) //每读取256字节数据换行
    {
      Serial.println("");
    }
  }

  Serial.println("End read");
}

void loop()
{}
