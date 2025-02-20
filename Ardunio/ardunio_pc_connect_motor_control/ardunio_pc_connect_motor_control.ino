int motorPin1 = 3; // Motor sürücüsünün IN1 pini
int motorPin2 = 4; // Motor sürücüsünün IN2 pini
int enPin = 5;     // Motor sürücüsünün Enable pini (PWM)

void setup() {
  Serial.begin(9600); // Bilgisayar ile seri haberleşme
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(enPin, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read(); // Bilgisayardan gelen komutu oku

    if (command == 'f') { // İleri komutu
      digitalWrite(motorPin1, HIGH);
      digitalWrite(motorPin2, LOW);
      analogWrite(enPin, 255); // Hız (0-255)
    } else if (command == 'b') { // Geri komutu
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, HIGH);
      analogWrite(enPin, 255); // Hız (0-255)
    } else if (command == 's') { // Durdurma komutu
      analogWrite(enPin, 0); // Motoru durdur
    }
  }
}
