#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Servo.h>

Adafruit_MPU6050 mpu;
Servo myServo;  // Servo nesnesi oluştur

const int servoPin = 9;  // Servonun bağlı olduğu pin
const float minAngle = 0.0;  // Minimum servo açısı
const float maxAngle = 180.0;  // Maximum servo açısı
const float minAccel = -9.8;  // Minimum ivme (yaklaşık -1g)
const float maxAccel = 9.8;   // Maximum ivme (yaklaşık 1g)

void setup(void) {
  Serial.begin(115200);
  
  // MPU6050'yi başlat
  if (!mpu.begin()) {x
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  
  // MPU6050 ayarları
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
  // Servoyu başlat
  myServo.attach(servoPin);
  
  delay(100);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  // Y eksenindeki ivmeye göre servo açısını hesapla
  float servoAngle = map(a.acceleration.y, minAccel, maxAccel, minAngle, maxAngle);
  servoAngle = constrain(servoAngle, minAngle, maxAngle);
  
  // Servoyu hareket ettir
  myServo.write(servoAngle);
  
  // Sensör verilerini ve servo açısını yazdır
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");
  
  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");
  
  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");
  
  Serial.print("Servo Angle: ");
  Serial.println(servoAngle);
  
  Serial.println("");
  delay(100);  // Daha hızlı güncelleme için gecikmeyi azalttık
}