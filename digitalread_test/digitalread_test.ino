#include <gpio.h>

#define gpio_pin 14

void setup() {
  Serial.begin(115200);
}


void loop() {
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
  int val = GPIO_INPUT_GET(GPIO_ID_PIN(gpio_pin));
  Serial.println("-------------");
  Serial.println((String) val);
  delay(500);
}
