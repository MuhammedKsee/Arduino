#include <DHT.h>

#define DHTPIN 7
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  // put your setup code here, to run once:
}

void loop() {

  delay(2000);

  float h = dht.readHumidity();

  float t = dht.readTemperature();

  float f = dht.readTemperature(true);


  Serial.print("Nem : ");
  Serial.print(h);
  Serial.print(" %t");
  Serial.print("Sıcaklık : ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\n");

}
