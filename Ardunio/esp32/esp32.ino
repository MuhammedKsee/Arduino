#include <WiFi.h>
#include <Arduino.h>
#include <SPIFFS.h>

// Wi-Fi bilgileri
const char* ssid = "Eslem";
const char* password = "eslem2003";

// Pin tanımlamaları
const int EKG_PIN = 34;
const int LO_PLUS_PIN = 35;
const int LO_MINUS_PIN = 32;

WiFiServer server(80);

bool capturing = false;
File dataFile;

void setup() {
  Serial.begin(115200);

  // Wi-Fi'ye bağlan
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Wi-Fi'ye bağlanılıyor...");
  }
  Serial.println("Wi-Fi bağlantısı kuruldu!");
  Serial.print("IP Adresi: ");
  Serial.println(WiFi.localIP());

  // Pin modlarını ayarla
  pinMode(LO_PLUS_PIN, INPUT);
  pinMode(LO_MINUS_PIN, INPUT);

  // SPIFFS başlat
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS başlatılamadı.");
    return;
  }

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("Yeni istemci bağlandı.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(
              "<!DOCTYPE HTML>"
              "<html>"
              "<head>"
              "<title>EKG Sensör Kontrol</title>"
              "<meta name='viewport' content='width=device-width, initial-scale=1'>"
              "<style>"
              "body { font-family: Arial; text-align: center; margin:0px auto; padding-top: 30px; }"
              ".button { padding: 10px 20px; font-size: 24px; text-align: center; outline: none; color: #fff; background-color: #2f4468; border: none; border-radius: 5px; box-shadow: 0 6px #999; cursor: pointer; }"
              ".button:hover {background-color: #1f2e45}"
              ".button:active { background-color: #1f2e45; box-shadow: 0 4px #666; transform: translateY(2px); }"
              "</style>"
              "</head>"
              "<body>"
              "<h1>EKG Sensör Kontrol</h1>"
              "<p><a href='/toggle'><button class='button'>Veri Al / Durdur</button></a></p>"
              "<p><a href='/download'><button class='button'>Dosya İndir</button></a></p>"
              "<p id='status'>Durum: "
            );
            client.print(capturing ? "Veri Alınıyor" : "Bekleniyor");
            client.print(
              "</p>"
              "</body>"
              "</html>"
            );
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        if (currentLine.endsWith("GET /toggle")) {
          capturing = !capturing;
          if (capturing) {
            dataFile = SPIFFS.open("/ekg_data.txt", FILE_WRITE);
          } else if (dataFile) {
            dataFile.close();
          }
        }

        if (currentLine.endsWith("GET /download")) {
          if (SPIFFS.exists("/ekg_data.txt")) {
            File file = SPIFFS.open("/ekg_data.txt", FILE_READ);
            if (file) {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/plain");
              client.println("Content-Disposition: attachment; filename=ekg_data.txt");
              client.println();
              while (file.available()) {
                client.write(file.read());
              }
              file.close();
              break;
            }
          }
        }
      }
    }
    client.stop();
    Serial.println("İstemci bağlantısı kesildi.");
  }

  if (capturing) {
    int ekgValue = analogRead(EKG_PIN);
    bool leadOff = digitalRead(LO_PLUS_PIN) == HIGH || digitalRead(LO_MINUS_PIN) == HIGH;
    int loStatus = leadOff ? 1 : 0;
    String data = "EKG: " + String(ekgValue) + ", LO Durumu: " + String(loStatus) + "\n";
    if (dataFile) {
      dataFile.print(data);
    }
    delay(100); // Veri alma hızını ayarlamak için
  }
}