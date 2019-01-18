#include "microthings8266.h"

MicroThings8266 mt8266;

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Initializing");
  mt8266.setup();
}

void loop() {
  mt8266.loop();  
}
