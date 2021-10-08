#include <Wire.h>
#include <gpio.h>

#define gpio_pin 14
#define LOW 0
#define HIGH 1

/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-button-long-press-short-press
 */

// constants won't change. They're used here to set pin numbers:
const int SHORT_PRESS_TIME = 1000; // 1000 milliseconds
const int LONG_PRESS_TIME  = 1000; // 1000 milliseconds

// Variables will change:
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;


void setup() {
  Serial.begin(115200);
  Serial.println("init...........");
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
}

void loop() {
  // read the state of the switch/button:
  currentState = GPIO_INPUT_GET(GPIO_ID_PIN(gpio_pin));

  if(lastState == LOW && currentState == HIGH)        // button is pressed
    pressedTime = millis();
  else if(lastState == HIGH && currentState == LOW) { // button is released
    releasedTime = millis();

    long pressDuration = releasedTime - pressedTime;

    if( pressDuration < SHORT_PRESS_TIME )
      Serial.println("a SHORT press is detected");

    if( pressDuration > LONG_PRESS_TIME )
      Serial.println("a LONG press is detected");
  }

  // save the the last state
  lastState = currentState;
  delay(1);
}
