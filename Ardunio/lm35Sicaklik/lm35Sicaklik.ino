int analogPin = A0;
float deger,sicaklik;
void setup() {
Serial.begin(9600);
}

void loop(){
  
  deger = analogRead(analogPin);

  deger = (deger*5000)/1023;

  sicaklik = deger/10.0;

  Serial.print("Sicaklik : ");
  Serial.print(sicaklik);
  Serial.print(" derece\n");
  delay(1000);

}