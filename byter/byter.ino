#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
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

#define VALID_MIN_USAGE_ID 4
#define VALID_MAX_USAGE_ID 99

unsigned int cnt = 0;

int addr = 1;

// WiFi
const char *ssid = "byter_epad";
const char *password = "dca632240c1c";

// MQTT Broker
const char *mqtt_broker = "10.3.141.1";
const char *out_topic = "byter/v1.0";
String in_topic_prefix = "byter/setcnt/";
String in_topic = "";
const char *mqtt_username = "byter_000001";
const char *mqtt_password = "";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

String helloText[3] = {"Keyboard", "Stokes", "Counter"};

Ticker writeCntTicker;
Ticker sendCntTicker;

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

int last_v3 = 0;
int last_v4 = 0;
int last_v5 = 0;
int last_v6 = 0;
int last_v7 = 0;
int last_v8 = 0;


int writeCnt() {
  EEPROM.begin(5);
  EEPROM.put(addr, cnt);
  int res = EEPROM.commit();
  EEPROM.end();
  Serial.println("write cnt");
  return res;
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
  EEPROM.begin(5);
  EEPROM.get(addr, _cnt);
  EEPROM.end();
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
      if (Serial.read() == 0x57) {
        delay(1);
        if (Serial.read() == 0xAB) {
          delay(1);
          if (Serial.read() == 0x83){
            delay(1);
            if (Serial.read() == 0x0C){
              delay(1);
              if (Serial.read() == 0x12){
                delay(1);
                if (Serial.read() == 0x01){
                  delay(1);
                  int v1 = Serial.read();
                  Serial.println("-------------start");
                  Serial.print("v1: ");
                  Serial.println(v1);
                  delay(1);
                  int v2 = Serial.read();
                  Serial.print("v2: ");
                  Serial.println(v2);
                  delay(1);

                  // 以下为有效键值
                  int v3 = Serial.read();
                  Serial.print("v3: ");
                  Serial.println(v3);
                  delay(1);
                  int v4 = Serial.read();
                  Serial.print("v4: ");
                  Serial.println(v4);
                  delay(1);
                  int v5 = Serial.read();
                  Serial.print("v5: ");
                  Serial.println(v5);
                  delay(1);
                  int v6 = Serial.read();
                  Serial.print("v6: ");
                  Serial.println(v6);
                  delay(1);
                  int v7 = Serial.read();
                  Serial.print("v7: ");
                  Serial.println(v7);
                  delay(1);
                  int v8 = Serial.read();
                  Serial.print("v8: ");
                  Serial.println(v8);

                  int incr = 0;

                  if (v3 >= VALID_MIN_USAGE_ID && v3 <= VALID_MAX_USAGE_ID && last_v3 != v3) {
                    last_v3 = v3;
                  } else if (v3 == 0 && last_v3 != 0) {
                    incr ++;
                    last_v3 = 0;
                  }

                  if (v4 >= VALID_MIN_USAGE_ID && v4 <= VALID_MAX_USAGE_ID && last_v4 != v4) {
                    last_v4 = v4;
                  } else if (v4 == 0 && last_v4 != 0) {
                    incr ++;
                    last_v4 = 0;
                  }

                  if (v5 >= VALID_MIN_USAGE_ID && v5 <= VALID_MAX_USAGE_ID && last_v5 != v5) {
                    last_v5 = v5;
                  } else if (v5 == 0 && last_v5 != 0) {
                    incr ++;
                    last_v5 = 0;
                  }

                  if (v6 >= VALID_MIN_USAGE_ID && v6 <= VALID_MAX_USAGE_ID && last_v6 != v6) {
                    last_v6 = v6;
                  } else if (v6 == 0 && last_v6 != 0) {
                    incr ++;
                    last_v6 = 0;
                  }

                  if (v7 >= VALID_MIN_USAGE_ID && v7 <= VALID_MAX_USAGE_ID && last_v7 != v7) {
                    last_v7 = v7;
                  } else if (v7 == 0 && last_v7 != 0) {
                    incr ++;
                    last_v7 = 0;
                  }

                  if (v8 >= VALID_MIN_USAGE_ID && v8 <= VALID_MAX_USAGE_ID && last_v8 != v8) {
                    last_v8 = v8;
                  } else if (v8 == 0 && last_v8 != 0) {
                    incr ++;
                    last_v8 = 0;
                  }

                  Serial.println(incr);

                  // 单击按键较低概率会出现v7与v3重复情况, 原因未知
                  if (v3 != 0 && v3 == v7) {
                    cnt --;
                  }
                  if (v4 != 0 && v4 == v8) {
                    cnt --;
                  }

                  if (incr) {
                    cnt = cnt + incr;
                    displayNum(cnt);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

void turnOnLed() {
  digitalWrite(D6, HIGH);
}

void turnOffLed() {
  digitalWrite(D6, LOW);
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
  delay(5000);
}

void  connWifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
      String client_id = String(WiFi.macAddress());
      // String client_id = "byter_cli_0001";
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("Public emqx mqtt broker connected");
          client.publish(out_topic, "hello, msg from esp8266");
          in_topic = in_topic_prefix + client_id;
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
  client.subscribe(in_topic.c_str());
}

void sendCnt() {
  // 每5秒调用一次, 发送MQTT消息
  digitalWrite(D6, HIGH);

  Serial.println("ticker send mqtt");
  String cnt_str = (String) cnt;
  client.publish(out_topic, cnt_str.c_str());

  digitalWrite(D6, LOW);
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  StaticJsonDocument<256> doc;
  deserializeJson(doc, payload, length);
  cnt = doc["cnt"];
  Serial.println();
  Serial.println("-----------------------");
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
  pinMode(D6, OUTPUT);

  turnOffLed();
  
  cnt = readCnt();
  Serial.println("setup readCnt");
  Serial.println(cnt);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  startDisplay();

  displayNum(cnt);

  writeCntTicker.attach(1800, writeCnt);

  connWifi();

  sendCntTicker.attach(5, sendCnt);
}

// loop
void loop() {
  client.loop();

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
      // 功能待定
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
