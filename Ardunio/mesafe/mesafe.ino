int trigPin = 12;
int echoPin = 11;
const int Enable_A = 9;
const int Enable_B = 10;
const int inputA1 = 2;
const int inputA2 = 3;
const int inputB1 = 4;
const int inputB2 = 5;
#include <LiquidCrystal_I2C_AvrI2C.h>

// LCD 16x2 ekran nesnesi
LiquidCrystal_I2C_AvrI2C lcd(0x27, 16, 2);  

void setup() {
    pinMode(Enable_A, OUTPUT);
  pinMode(Enable_B, OUTPUT);
  pinMode(inputA1, OUTPUT);
  pinMode(inputA2, OUTPUT);
  pinMode(inputB1, OUTPUT);
  pinMode(inputB2, OUTPUT);
  // Pin modlarını belirliyoruz
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);

  // LCD'yi başlatıyoruz ve arka ışığını açıyoruz
  lcd.begin();
  lcd.backlight();
  lcd.clear(); // LCD ekranını temizliyoruz
}

void loop() {
  int sure, uzaklik;
  digitalWrite(Enable_A, HIGH);
  digitalWrite(Enable_B, HIGH);
  // Trig pini ile sinyal gönderiyoruz
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(500);
  digitalWrite(trigPin, LOW);

  // Echo pini ile dönüş süresini ölçüyoruz
  sure = pulseIn(echoPin, HIGH);

  // Mesafeyi cm cinsinden hesaplıyoruz
  uzaklik = sure / 58;
  if (uzaklik < 0)
    uzaklik = 100;
  Serial.print("Uzaklik(cm): ");
  Serial.println(uzaklik);

  // LCD ekranına mesafeyi yazdırıyoruz
  lcd.setCursor(0, 0); // 1. satır, 1. karakter
  lcd.print("Mesafe: ");
  lcd.print(uzaklik);
  lcd.print(" cm");

  // Mesafe 20 cm'den az ise uyarı veriyoruz
  lcd.setCursor(0, 1); // 2. satır, 1. karakter
  if (uzaklik < 20) {
    lcd.print("Dur Cok Yakin!");
  } else {
    lcd.clear(); // Mesajı temizliyoruz
    lcd.setCursor(0, 0); // İlk satırdaki mesafeyi yeniden yazıyoruz
    lcd.print("Mesafe: ");
    lcd.print(uzaklik);
    lcd.print(" cm");
  }
  if (uzaklik > 20){
    digitalWrite(inputA1, HIGH);
    digitalWrite(inputA2, LOW);
    digitalWrite(inputB1, HIGH);
    digitalWrite(inputB2, LOW);
    }
  if (uzaklik <=20){
    digitalWrite(Enable_A, LOW);
    digitalWrite(Enable_B, LOW);
    digitalWrite(Enable_A, HIGH);
    digitalWrite(Enable_B, HIGH);
    digitalWrite(inputA1, HIGH);
    digitalWrite(inputA2, LOW);
    digitalWrite(inputB1, LOW);
    digitalWrite(inputB2, HIGH);
  }

  delay(1000); // Biraz bekleme süresi ekliyoruz
}
