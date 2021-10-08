#include <gpio.h>

#define gpio_pin 12

void setup() {
  Serial.begin(115200);
  // PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
  // GPIO_OUTPUT_SET(GPIO_ID_PIN(gpio_pin), 1);

  pinMode(D6, OUTPUT);
  digitalWrite(D6, HIGH);
}


void loop() {
  // GPIO_OUTPUT_SET(GPIO_ID_PIN(gpio_pin), 0);
  // digitalWrite(D6, HIGH);
  // delay(1000);
  // GPIO_OUTPUT_SET(GPIO_ID_PIN(gpio_pin), 1);
  // digitalWrite(D6, LOW);
  // delay(1000);
//   GPIO_OUTPUT_SET(GPIO_ID_PIN(gpio_pin), 1);
//   Serial.println("-------------on");
//   delay(4000);
//   GPIO_OUTPUT_SET(GPIO_ID_PIN(gpio_pin), 0);
//   Serial.println("-------------off");
//   delay(4000);
}
