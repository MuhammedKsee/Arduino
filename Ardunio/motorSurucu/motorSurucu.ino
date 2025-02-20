const int Enable_A = 9;
const int Enable_B = 10;
const int inputA1 = 2;
const int inputA2 = 3;
const int inputB1 = 4;
const int inputB2 = 5;

void setup() {
  pinMode(Enable_A, OUTPUT);
  pinMode(Enable_B, OUTPUT);
  pinMode(inputA1, OUTPUT);
  pinMode(inputA2, OUTPUT);
  pinMode(inputB1, OUTPUT);
  pinMode(inputB2, OUTPUT);
}

void loop() {
  //---- A ve B Çıkış olarak etkinleştir ------//
  digitalWrite(Enable_A, HIGH);
  digitalWrite(Enable_B, HIGH);

  //---------- Motorları çalıştır -----------//
  digitalWrite(inputA1, HIGH);
  digitalWrite(inputA2, LOW);
  digitalWrite(inputB1, HIGH);
  digitalWrite(inputB2, LOW);
  delay(3000);

  //------- Motoru devre dışı bırak ----------//
  digitalWrite(Enable_A, LOW);
  digitalWrite(Enable_B, LOW);
  delay(3000);

  //------- Ters bağlantı ----------//
  digitalWrite(Enable_A, HIGH);
  digitalWrite(Enable_B, HIGH);
  digitalWrite(inputA1, LOW);
  digitalWrite(inputA2, HIGH);
  digitalWrite(inputB1, LOW);
  digitalWrite(inputB2, HIGH);
  delay(3000);

  //------- Motoru devre dışı bırak ----------//
  digitalWrite(Enable_A, LOW);
  digitalWrite(Enable_B, LOW);
  delay(3000);

  //---------- Hız yükselt ----------//
  for (int i = 0; i < 256; i++) {
    analogWrite(Enable_A, i);
    analogWrite(Enable_B, i);
    delay(40);
  }

  //---------- Hız düşür ----------//
  for (int j = 255; j >= 0; j--) {
    analogWrite(Enable_A, j);
    analogWrite(Enable_B, j);
    delay(40);
  }

  //------- Motoru devre dışı bırak ----------//
  digitalWrite(Enable_A, LOW);
  digitalWrite(Enable_B, LOW);
  delay(3000);
}
