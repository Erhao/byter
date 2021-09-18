#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Ticker.h>
#include <EEPROM.h> 

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// #define EEPROM_write(address, stru) {byte *p_stru = (byte*)&(stru);for(int i = 0;i<sizeof(stru);i++){ EEPROM.write(address+i,p_stru[i]);delay(4);}EEPROM.commit();}//EEPROM.end();
// #define EEPROM_read(address,stru) {byte *p_stru = (byte*)&(stru);for(int i = 0;i<sizeof(stru);i++){ p_stru[i]=char(EEPROM
#define EEPROM_ADDR 1

Ticker writeCountTicker;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

AsyncWebServer server(80);

const char *SSID = "KeyboardStokesCounter";
const char *PASSWORD = "12345678";

int cnt;

void writeCount() {
  EEPROM.put(EEPROM_ADDR, cnt);
}

int readCount() {
  int _cnt;
  EEPROM.get(EEPROM_ADDR, _cnt);
  return _cnt;
}

void clearCount() {
  EEPROM.begin(10);
  // write a 0 to all 10 bytes of the EEPROM
  for (int i = 0; i < 10; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
}

void rw() {
  // CH9350有效键值帧组成如下:
  /*
  帧头 2字节 固定为:0x57 0xAB  (对应十进制87 171)
  命令码 1字节 用于辨别是有效键值帧的码值:0x83 / 0x88 (对应十进制131 136) 其中上位机模式和下位机模式工作在状态1时命令码为0x83;
    下位机模式工作在状态0时命令码为0x88
  长度 1字节 后续数据(标识+键值+序列号+校验)长度值
  键值 varible 键盘或鼠标上传的数据
  序列号 1字节 数据帧序列号
  校验 1字节 1字节累加和校验(键值+序列号)
  */
  while (Serial.available() > 0) {
    //57 AB 83 0C 12 01 00 00 04 00 00 00 00 00 12 17
    if (Serial.read() == 0x83){
      delay(10);
      if (Serial.read() == 0x0C){
        delay(10);
        if (Serial.read() == 0x12){
          delay(10);
          if (Serial.read() == 0x01){
            delay(10);
            Serial.read();
            delay(10);
            Serial.read();
            delay(10);

            int val = Serial.read();
            if (val != 0x00) {
              Serial.println(val);
              Serial.println(getKey(val));
              Serial.println("------------");
              if (val >= 4 && val <= 83) {
                cnt ++;
              }
              Serial.print("cnt: ");
              Serial.println(cnt);

              display.clearDisplay();
              display.setTextColor(WHITE);
              if (cnt <= 99999) {
                display.setCursor(0, 2);
                display.setTextSize(4);
              } else if (cnt <= 9999999) {
                display.setCursor(0, 5);
                display.setTextSize(3);
              } else if (cnt <= 9999999999) {
                display.setCursor(0, 9);
                display.setTextSize(2);
              }
              display.println((String) cnt);
              display.display();
            }
          }
        }
      }
    }
  }
}

void startServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    int _cnt = readCount();
    String text = "Up to now, " + (String) _cnt + " keyboard strokes have been recorded.";
    request->send(200, "text/plain", text);
  });

  server.on("/clear", HTTP_GET, [](AsyncWebServerRequest *request){
    clearCount();
    request->send(200, "text/plain", "KeyStoke Count Cleared!");
  });
  
  server.begin();
}

String getKey(int serialData){
  if(serialData==0x00){return "";}
  if(serialData==0x04){return "A";}
  if(serialData==0x05){return "B";}
  if(serialData==0x06){return "C";}
  if(serialData==0x07){return "D";}
  if(serialData==0x08){return "E";}
  if(serialData==0x09){return "F";}
  if(serialData==0x0A){return "G";}
  if(serialData==0x0B){return "H";}
  if(serialData==0x0C){return "I";}
  if(serialData==0x0D){return "J";}
  if(serialData==0x0E){return "K";}
  if(serialData==0x0F){return "L";}
  if(serialData==0x10){return "M";}
  if(serialData==0x11){return "N";}
  if(serialData==0x12){return "O";}
  if(serialData==0x13){return "P";}
  if(serialData==0x14){return "Q";}
  if(serialData==0x15){return "R";}
  if(serialData==0x16){return "S";}
  if(serialData==0x17){return "T";}
  if(serialData==0x18){return "U";}
  if(serialData==0x19){return "V";}
  if(serialData==0x1A){return "W";}
  if(serialData==0x1B){return "X";}
  if(serialData==0x1C){return "Y";}
  if(serialData==0x1D){return "Z";}
  if(serialData==0x1E){return "[1 or !]";}
  if(serialData==0x1F){return "[2 or @]";}
  if(serialData==0x20){return "[3 or #]";}
  if(serialData==0x21){return "[4 or $]";}
  if(serialData==0x22){return "[5 or %]";}
  if(serialData==0x23){return "[6 or ^]";}
  if(serialData==0x24){return "[7 or &]";}
  if(serialData==0x25){return "[8 or *]";}
  if(serialData==0x26){return "[9 or (]";}
  if(serialData==0x27){return "[10 or )]";}
  if(serialData==0x28){return "[ENTER]";}
  if(serialData==0x29){return "[ESC]";}
  if(serialData==0x2A){return "[BACKSPACE]";}
  if(serialData==0x2B){return "[TAB]";}
  if(serialData==0x2C){return "[SPACE]";}
  if(serialData==0x2D){return "[- or _]";}
  if(serialData==0x2E){return "[= or +]";}
  if(serialData==0x2F){return "[[ or {]";}
  if(serialData==0x30){return "[] or }]";}
  if(serialData==0x31){return "[\\ or |]";}
  if(serialData==0x32){return "[` or ~]";}
  if(serialData==0x33){return "[; or :]";}
  if(serialData==0x34){return "[' or ”]";}
  if(serialData==0x35){return "[~ or `]";}
  if(serialData==0x36){return "[, or <]";}
  if(serialData==0x37){return "[. or >]";}
  if(serialData==0x38){return "[/ or ?]";}
  if(serialData==0x39){return "[CAPS]";}
  if(serialData==0x3A){return "[F1]";}
  if(serialData==0x3B){return "[F2]";}
  if(serialData==0x3C){return "[F3]";}
  if(serialData==0x3D){return "[F4]";}
  if(serialData==0x3E){return "[F5]";}
  if(serialData==0x3F){return "[F6]";}
  if(serialData==0x40){return "[F7]";}
  if(serialData==0x41){return "[F8]";}
  if(serialData==0x42){return "[F9]";}
  if(serialData==0x43){return "[F10]";}
  if(serialData==0x44){return "[F11]";}
  if(serialData==0x45){return "[F12]";}
  if(serialData==0x46){return "[PRT_SCR]";}
  if(serialData==0x47){return "[SCOLL_LOCK]";}
  if(serialData==0x48){return "[PAUSE]";}
  if(serialData==0x49){return "[INS]";}
  if(serialData==0x4A){return "[HOME]";}
  if(serialData==0x4B){return "[PAGEUP]";}
  if(serialData==0x4C){return "[DEL]";}
  if(serialData==0x4D){return "[END]";}
  if(serialData==0x4E){return "[PAGEDOWN]";}
  if(serialData==0x4F){return "[RIGHT_ARROW]";}
  if(serialData==0x50){return "[LEFT_ARROW]";}
  if(serialData==0x51){return "[DOWN_ARROW]";}
  if(serialData==0x52){return "[UP_ARROW]";}
  if(serialData==0x53){return "[PAD_NUMLOCK]";}
  if(serialData==0x54){return "[PAD_DIV]";}
  if(serialData==0x55){return "[PAD_MUL]";}
  if(serialData==0x56){return "[PAD_SUB]";}
  if(serialData==0x57){return "[PAD_ADD]";}
  if(serialData==0x58){return "[PAD_ENTER]";}
  if(serialData==0x59){return "[PAD_1]";}
  if(serialData==0x5A){return "[PAD_2]";}
  if(serialData==0x5B){return "[PAD_3]";}
  if(serialData==0x5C){return "[PAD_4]";}
  if(serialData==0x5D){return "[PAD_5]";}
  if(serialData==0x5E){return "[PAD_6]";}
  if(serialData==0x5F){return "[PAD_7]";}
  if(serialData==0x60){return "[PAD_8]";}
  if(serialData==0x61){return "[PAD_9]";}
  if(serialData==0x62){return "[PAD_0]";}
  if(serialData==0x63){return "[PAD_DOT]";}
  if(serialData==0xE0){return "[leftctrl]";}
  if(serialData==0xE2){return "[leftAlt]";}
  if(serialData==0xE1){return "[leftShift]";}
  if(serialData==0xE3){return "[leftwindows]";}
  if(serialData==0xE7){return "[rightwindows]";}
  if(serialData==0xE5){return "[rightShift]";}
  if(serialData==0xE6){return "[rightAlt]";}
  if(serialData==0xE4){return "[rightCtrl]";}
}

// main
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  cnt = 0;

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  WiFi.mode(WIFI_STA);
  WiFi.softAP(SSID, PASSWORD);

  startServer();

  writeCountTicker.attach(10, writeCount);
}

// loop
void loop() {
  // put your main code here, to run repeatedly:
  rw();
}
