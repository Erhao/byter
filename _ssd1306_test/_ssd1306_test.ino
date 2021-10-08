#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  int cnt = 6789;
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(4);
//  display.setTextColor(BLACK, WHITE); //反白，白底黑字
  display.setCursor(0, 0);
  display.print("56789");
//  display.println((String) cnt);
  display.display();
}

void loop() {
  // put your main code here, to run repeatedly:

}
