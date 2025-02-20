#include "esp_camera.h"
#include <WiFi.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_http_server.h"
#include <ESP32Servo.h>

// Wi-Fi bilgileri
const char* ssid = "FiberHGW_VE1A21_2.4GHz";
const char* password = "Jky4ADmHdpTr";

#define CAMERA_MODEL_AI_THINKER

// AI Thinker pin tanımlamaları
#if defined(CAMERA_MODEL_AI_THINKER)
  #define PWDN_GPIO_NUM     32
  #define RESET_GPIO_NUM    -1
  #define XCLK_GPIO_NUM      0
  #define SIOD_GPIO_NUM     26
  #define SIOC_GPIO_NUM     27
  #define Y9_GPIO_NUM       35
  #define Y8_GPIO_NUM       34
  #define Y7_GPIO_NUM       39
  #define Y6_GPIO_NUM       36
  #define Y5_GPIO_NUM       21
  #define Y4_GPIO_NUM       19
  #define Y3_GPIO_NUM       18
  #define Y2_GPIO_NUM        5
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     23
  #define PCLK_GPIO_NUM     22
#endif

// Servo motor pin tanımlamaları
#define SERVO_1      14
#define SERVO_2      15
#define SERVO_STEP   5

Servo servo1;
Servo servo2;
int servo1Pos = 0;
int servo2Pos = 0;

static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=123456789000000000000987654321";
static const char* _STREAM_BOUNDARY = "\r\n--123456789000000000000987654321\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t camera_httpd = NULL;
httpd_handle_t stream_httpd = NULL;

static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="tr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Kamera Arayüzü</title>
    <style>
        body {
            margin: auto;
            padding: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            background-color: black;
            font-family: Arial, sans-serif;
        }
        .device {
            width: 300px;
            height: 600px;
            background: linear-gradient(to bottom, #ffd700 0%, #ffd700 30%, #000000 70%, #000000 100%);
            border-radius: 20px;
            display: flex;
            flex-direction: column;
            align-items: center;
            padding: 20px;
            box-shadow: 0 0 10px rgba(255,255,255,0.1);
            margin: auto;
        }
        .logo {
            font-size: 48px;
            font-weight: bold;
            margin-bottom: 20px;
            color: black;
        }
        .camera-container {
            width: 250px;
            height: 350px;
            background-color: transparent;
            border-radius: 10px;
            position: relative;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            padding-top: 30px;
        }
        .camera-view {
            width: 230px;
            height: 230px;
            background-color: none;
            border: 2px solid #333;
            border-radius: 10px;
            position: relative;
        }
        .label {
            position: absolute;
            top: 10px;
            left: 50%;
            transform: translateX(-50%);
            font-weight: bold;
            color: white;
            padding: 5px;
            background: #ffd700;
            border-radius: 10px;
            z-index: 2;
            text-align: center;
            width: 100%;
        }
        .controls {
            width: 150px;
            height: 150px;
            position: relative;
            margin-top: 40px;
        }
        .control-button {
            width: 0;
            height: 0;
            border-style: solid;
            position: absolute;
            cursor: pointer;
            border-radius: 5px;
        }
        .up {
            border-width: 0 20px 30px 20px;
            border-color: transparent transparent #333 transparent;
            top: 0;
            left: 50%;
            transform: translateX(-50%);
        }
        .down {
            border-width: 30px 20px 0 20px;
            border-color: #333 transparent transparent transparent;
            bottom: 0;
            left: 50%;
            transform: translateX(-50%);
        }
        .left {
            border-width: 20px 30px 20px 0;
            border-color: transparent #333 transparent transparent;
            top: 50%;
            left: 0;
            transform: translateY(-50%);
        }
        .right {
            border-width: 20px 0 20px 30px;
            border-color: transparent transparent transparent #333;
            top: 50%;
            right: 0;
            transform: translateY(-50%);
        }
        .control-label {
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            font-weight: bold;
            color: white;
        }
    </style>
</head>
<body>
    <div class="device">
        <div class="logo">Y</div>
        <div class="camera-container">
            <div class="label">Kamera Görüntüsü</div>
            <div class="camera-view">
                <img id="camera-stream" src="/stream" style="width: 100%; height: 100%; border: none;" />
            </div>
        </div>
        <div class="controls">
            <div class="control-button up"></div>
            <div class="control-button left"></div>
            <div class="control-button right"></div>
            <div class="control-button down"></div>
            <div class="control-label">Kontrol</div>
        </div>
    </div>

    <script>
        const buttons = document.querySelectorAll('.control-button');
        buttons.forEach(button => {
            button.addEventListener('click', function() {
                alert('Düğmeye tıklandı: ' + this.className);
                // Servo motor kontrol kodu buraya eklenebilir
            });
        });
    </script>
</body>
</html>
)rawliteral";

static esp_err_t index_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, (const char *)INDEX_HTML, strlen(INDEX_HTML));
}

static esp_err_t stream_handler(httpd_req_t *req) {
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t * _jpg_buf = NULL;
    char part_buf[64];

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK) {
        return res;
    }

    while (true) {
        fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("Camera capture failed");
            res = ESP_FAIL;
        } else {
            if (fb->format != PIXFORMAT_JPEG) {
                bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
                esp_camera_fb_return(fb);
                fb = NULL;
                if (!jpeg_converted) {
                    Serial.println("JPEG compression failed");
                    res = ESP_FAIL;
                }
            } else {
                _jpg_buf_len = fb->len;
                _jpg_buf = fb->buf;
            }
        }

        if (res == ESP_OK && _jpg_buf) {
            size_t hlen = snprintf(part_buf, sizeof(part_buf), _STREAM_PART, _jpg_buf_len);
            res = httpd_resp_send_chunk(req, part_buf, hlen);
            if (res == ESP_OK) {
                res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
                if (res == ESP_OK) {
                    res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
                }
            }
        }

        if (fb) {
            esp_camera_fb_return(fb);
        }
        if (_jpg_buf) {
            free(_jpg_buf);
        }

        if (res != ESP_OK) {
            break;
        }
    }
    return res;
}

static esp_err_t cmd_handler(httpd_req_t *req) {
    char* buf;
    size_t buf_len;
    buf_len = httpd_req_get_hdr_value_len(req, "cmd") + 1;
    if (buf_len > 1) {
        buf = (char*) malloc(buf_len);
        if (httpd_req_get_hdr_value_str(req, "cmd", buf, buf_len) == ESP_OK) {
            Serial.println(buf);
            // Servo motor kontrolü burada yapılabilir
            free(buf);
        }
    }
    httpd_resp_sendstr(req, "Command received");
    return ESP_OK;
}

void startCameraServer() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    config.ctrl_port = 32768;
    config.max_uri_handlers = 10;
    config.max_resp_headers = 20;
    config.max_open_sockets = 10;
    config.stack_size = 4096;

    // Web sunucusunu başlat
    if (httpd_start(&camera_httpd, &config) == ESP_OK) {
        httpd_uri_t index_uri = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = index_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(camera_httpd, &index_uri);

        httpd_uri_t stream_uri = {
            .uri = "/stream",
            .method = HTTP_GET,
            .handler = stream_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(camera_httpd, &stream_uri);

        httpd_uri_t cmd_uri = {
            .uri = "/cmd",
            .method = HTTP_GET,
            .handler = cmd_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(camera_httpd, &cmd_uri);
    }
}

void setup() {
    Serial.begin(115200);

    // Wi-Fi bağlantısını başlat
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Bağlandı!");

    // Kamera yapılandırmasını başlat
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    // Minimum çözünürlükte başlat
    if (psramFound()) {
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 10;
        config.fb_count = 2;
    } else {
        config.frame_size = FRAMESIZE_VGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    // Kamera başlat
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Kamera başlatılamadı. Hata 0x%x", err);
        return;
    }

    // Kamera sunucusunu başlat
    startCameraServer();
}

void loop() {
    // Burada herhangi bir işlem yapılmıyor
}
