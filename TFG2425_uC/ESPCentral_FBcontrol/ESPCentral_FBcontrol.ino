#include <esp_now.h>
#include <WiFi.h>
#include <defMsg/defMsg_types.h>
#include <esp_wifi.h>
#include <Firebase_ESP_Client.h>
#include <ESP32Time.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

msg_BME280 data_BME280;
bool pir_status = false;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

ESP32Time rtc;

// Configuración Wi-Fi (reemplazar)
const char* ssid = "TU_SSID";
const char* password = "TU_PASSWORD";

// Configuración NTP
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 1 * 3600;
const int daylightOffset_sec = 0;

// Firebase (reemplazar)
#define API_KEY "TU_API_KEY"
#define DATABASE_URL "https://TU_PROJECT.firebaseio.com/"

// Direcciones MAC de los sensores (reemplazar)
uint8_t mac_oficina[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t mac_cocina[]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t mac_pir[]     = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

volatile bool bme280_flag = false;
volatile bool pir_flag = false;
bool signupOK = false;
String sensor_location = "";

void OnDataRecv(const esp_now_recv_info_t* info, const uint8_t* incomingData, int len) {
  Serial.print("MAC del emisor: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", info->src_addr[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.println();

  if (memcmp(info->src_addr, mac_oficina, 6) == 0) {
    sensor_location = "Oficina";
    memcpy(&data_BME280, incomingData, sizeof(data_BME280));
    bme280_flag = true;

  } else if (memcmp(info->src_addr, mac_cocina, 6) == 0) {
    sensor_location = "Cocina";
    memcpy(&data_BME280, incomingData, sizeof(data_BME280));
    bme280_flag = true;

  } else if (memcmp(info->src_addr, mac_pir, 6) == 0) {
    memcpy(&pir_status, incomingData, sizeof(bool));
    pir_flag = true;

  } else {
    Serial.println("uC desconocido");
    return;
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi conectado");
  int wifiChannel = WiFi.channel();
  WiFi.disconnect();
  delay(200);
  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(wifiChannel, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al iniciar ESP-NOW");
  }

  esp_now_register_recv_cb(OnDataRecv);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));

  if (Firebase.signUp(&config, &auth, "", "")) {
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (bme280_flag) {
    String key = "BME280/" + sensor_location + "/" + String(rtc.getEpoch());

    FirebaseJson json;
    json.set("temperatura", data_BME280.temperatura);
    json.set("humedad", data_BME280.humitat);
    json.set("presion", data_BME280.pressio);

    if (Firebase.ready() && signupOK) {
      if (Firebase.RTDB.setJSON(&fbdo, key, &json)) {
        Serial.println("Datos enviados correctamente");
      } else {
        Serial.println("Error al enviar datos: " + fbdo.errorReason());
      }
    }

    bme280_flag = false;
    sensor_location = "";
  }

  if (pir_flag) {
    String key = "PIR";
    if (Firebase.ready() && signupOK) {
      if (Firebase.RTDB.setBool(&fbdo, key, pir_status)) {
        Serial.println("PIR enviado correctamente");
      } else {
        Serial.println("Error al enviar PIR: " + fbdo.errorReason());
      }
    }
    pir_flag = false;
  }
}
