#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Ticker.h>
#include <EEPROM.h> 
#include <gpio.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define BTN_PIN 14
#define LOW 0
#define HIGH 1
#define ONE_SECOND 1000
#define TEN_SECONDS 10000
#define TWENTY_SECONDS 20000

unsigned int cnt = 0;

int addr = 1;

String helloText[3] = {"Keyboard", "Stokes", "Counter"};

Ticker writeCntTicker;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

AsyncWebServer server(80);

const char *SSID = "KeyboardStokesCounter";
const char *PASSWORD = "12345678";

unsigned int maxCnt = 4294967295;
bool isCloseToMax = false;
bool isOverflow = false;

int lastState = LOW;
int curState;
unsigned long pressedMillis = 0;
unsigned long releasedMillis = 0;

bool isWifiOn = false;


int writeCnt() {
  EEPROM.put(addr, cnt);
  return EEPROM.commit();
}

void manualWriteCnt() {
  int writeRes = writeCnt();
  String text = "";
  if (writeRes) {
    text = "Saved !";
  } else {
    text = "Save Failed !";
  }
  displayText(text);
}

unsigned int readCnt() {
  unsigned int _cnt;
  EEPROM.get(addr, _cnt);
  return _cnt;
}

void clearCnt() {
  EEPROM.begin(5);
  // write a 0 to all 4 bytes of the EEPROM
  for (int i = 0; i < 5; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
}

void manualClearCnt() {
  clearCnt();
  cnt = 0;
  String text = "Cleared !";
  displayText(text);
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
  if (!isOverflow) {
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
                Serial.println("------------");
                if (val >= 4 && val <= 83) {
                  cnt ++;
                }
                Serial.print("cnt: ");
                Serial.println(cnt);
  
                // 刷新显示
                displayNum(cnt);
              }
            }
          }
        }
      }
    }
  }
}

void startServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    unsigned int _cnt = readCnt();
    String text = "Up to now, " + (String) _cnt + " keyboard strokes have been recorded.";
    request->send(200, "text/plain", text);
  });

  server.on("/clear", HTTP_GET, [](AsyncWebServerRequest *request){
    clearCnt();
    request->send(200, "text/plain", "KeyStoke Count Cleared!");
  });
  
  server.begin();
}

void enableWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.softAP(SSID, PASSWORD);
  isWifiOn = true;
  
  String text = "WiFi ON";
  displayText(text);
}

void disableWifi() {
  WiFi.mode(WIFI_OFF);
  isWifiOn = false;
  
  String text = "WiFi OFF";
  displayText(text);
}

void displayNum(unsigned int n) {
  display.clearDisplay();
  display.setTextColor(WHITE);
  if (n <= 99999) {
    display.setCursor(0, 2);
    display.setTextSize(4);
    display.println((String) n);
  } else if (n <= 9999999) {
    display.setCursor(0, 5);
    display.setTextSize(3);
    display.println((String) n);
  } else if (n < maxCnt) {
    display.setCursor(0, 9);
    if (isCloseToMax) {
      display.setTextSize(1);
      display.println("CloseToMAX..");
    } else {
      display.setTextSize(2);
    }
    display.println((String) n);
  } else {
    display.setTextSize(1);
    display.println("Overflowing");
    display.println((String) maxCnt);
    isOverflow = true;
  }
  display.display();
}

void displayText(String text) {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 3);
  display.setTextSize(1);
  display.println("-------------------");
  display.println(text);
  display.println("-------------------");
  display.display();
  delay(2000);
  displayNum(cnt);
}

void startDisplay() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(helloText[0]);
  display.setCursor(48, 10);
  display.println(helloText[1]);
  display.setCursor(85, 20);
  display.println(helloText[2]);
  display.display();
  delay(3000);
}

// main
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("");
  EEPROM.begin(256);
  for (int addr = 0; addr < 256; addr ++) {
    int data = EEPROM.read(addr);
    Serial.print(data);
    Serial.print(" ");
    delay(2);
  }
  Serial.println("End read");
  
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
  
  cnt = readCnt();
  Serial.println("setup readCnt");
  Serial.println(cnt);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  startDisplay();

  startServer();

  writeCntTicker.attach(1800, writeCnt);
}

// loop
void loop() {
  rw();

  // read the state of the switch/button:
  curState = GPIO_INPUT_GET(GPIO_ID_PIN(BTN_PIN));

  if (lastState == LOW && curState == HIGH)        // button is pressed
    pressedMillis = millis();
  else if (lastState == HIGH && curState == LOW) { // button is released
    releasedMillis = millis();
    long pressMillis = releasedMillis - pressedMillis;
    if (pressMillis < ONE_SECOND) {
      Serial.println("a SHORT press is detected");
      // 将cnt写入EEPROM
      manualWriteCnt();
    } else if (pressMillis < TEN_SECONDS) {
      Serial.println("a LONG press is detected");
      // 开关wifi
      if (isWifiOn) {
        disableWifi();
      } else {
        enableWifi();
      }
    } else if (pressMillis < TWENTY_SECONDS) {
      Serial.println("a MUCH LONGER press is detected");
      // 清空EEPROM中的cnt
      manualClearCnt();
    }
  }
  // save the the last state
  lastState = curState;
  delay(1);
}
