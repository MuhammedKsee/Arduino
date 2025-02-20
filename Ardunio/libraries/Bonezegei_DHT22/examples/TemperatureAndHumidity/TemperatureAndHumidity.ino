/*
  Read Temperature and Humidity
  DHT22 Library
  Author: Bonezegei (Jofel Batutay)
  Date : November 2023

  Tested using ESP32-WROOM32
*/

#include <Bonezegei_DHT22.h>

//param = DHT22 signal pin
Bonezegei_DHT22 dht(14);

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {

  if (dht.getData()) {                         // get All data from DHT22
    float tempDeg = dht.getTemperature();      // return temperature in celsius
    float tempFar = dht.getTemperature(true);  // return temperature in fahrenheit if true celsius of false
    int hum = dht.getHumidity();               // return humidity
    Serial.printf("Temperature: %0.1lf°C  %0.1lf°F Humidity:%d \n", tempDeg, tempFar, hum);
  }
  delay(2000);  
}
