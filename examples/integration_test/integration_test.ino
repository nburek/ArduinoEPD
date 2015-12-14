#include "epd.h"

using namespace EPD;

Display disp(Serial1, 2, 3); //These inputs are based off pluggin the panel into pins 0-3 of the Arduino Yun

void setup() {
  disp.reset();
  disp.wakeUp();
  disp.setBaudRate(57600);
  
  Serial.begin(115200);
  Serial.println("Starting Integration tests");
}

void loop() {

  Serial.println("Loop Started");
  while (1) {
    delay(500);
  }
}
