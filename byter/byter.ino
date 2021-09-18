#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Ticker.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Ticker writeFileTicker;
Ticker checkIsCloseToMaxTicker;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

AsyncWebServer server(80);

File logFile;

const char *ssid = "KeyboardStokesCounter";
const char *password = "12345678";

unsigned long cnt;
int maxLength = sizeof(unsigned long);
bool isCloseToMax = false;
bool isOverflow = false;

/*
ticker task: save cnt to SPIFFS
*/
void cronSaveCount() {
  logFile.close();
  logFile = SPIFFS.open("/keyCount.txt", "w");
  String text = (String) cnt;
  logFile.print(text);
}

/*
ticker task: check if cnt is close to max
*/
void cronCheckIsCloseToMax() {
  if (maxLength - cnt < 1000) {
    isCloseToMax = true;
  }
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
                  display.clearDisplay();
                  display.setTextColor(WHITE);
                  if (cnt <= 99999) {
                    display.setCursor(0, 2);
                    display.setTextSize(4);
                    display.println((String) cnt);
                  } else if (cnt <= 9999999) {
                    display.setCursor(0, 5);
                    display.setTextSize(3);
                    display.println((String) cnt);
                  } else if (cnt < maxLength) {
                    display.setCursor(0, 9);
                    if (isCloseToMax) {
                      display.setTextSize(1);
                      display.println("CloseToMAX..");
                    } else {
                      display.setTextSize(2);
                    }
                    display.println((String) cnt);
                  } else {
                    display.setTextSize(1);
                    display.println("Overflowing");
                    display.println((String) maxLength);
                    isOverflow = true;
                  }
                  display.display();
                }
              }
            }
          }
        }
      }
  }
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
  WiFi.softAP(ssid,password);
  SPIFFS.begin();
  logFile = SPIFFS.open("/keyCount.txt", "a+");
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    logFile = SPIFFS.open("/keyCount.txt", "r");
    String cntString = logFile.readStringUntil('\n');
    String text = "Up to now, " + cntString + " keyboard strokes have been recorded.";
    request->send(200, "text/plain", text);
  });

  server.on("/clear", HTTP_GET, [](AsyncWebServerRequest *request){
    logFile.close();
    logFile = SPIFFS.open("/keyCount.txt", "w");
    request->send(200, "text/plain", "KeyStoke Count Cleared!");
  });
  
  server.begin();

  writeFileTicker.attach(10, cronSaveCount);
  checkIsCloseToMaxTicker.attach(120, cronCheckIsCloseToMax);
}

// loop
void loop() {
  // put your main code here, to run repeatedly:
  rw();
}
