#include<Servo.h>

int potpin = A0;
Servo servo;
int val = 0;

void setup() {
  // put your setup code here, to run once:
  servo.attach(9);
}

void loop() {
  // put your main code here, to run repeatedly:
  val = analogRead(potpin);
  val = map(val,0,1023,0,179);
  servo.write(val);
  delay(15);
}
