#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <esp_now.h>
#include <WiFi.h>
#include <defMsg/defMsg_types.h>
#include <esp_wifi.h>

msg_BME280 dades;
Adafruit_BME280 bme280;
esp_now_peer_info_t peerInfo;
uint8_t recieverAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };  // REEMPLAZAR con la MAC del receptor

volatile bool dataSent = false;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Estat d'enviament: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Missatge enviat correctament" : "Error enviant missatge");
  dataSent = (status == ESP_NOW_SEND_SUCCESS);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE);
  Serial.println("Canal WiFi forçat a 6");

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error inicialitzant ESP-NOW");
    return;
  }
  Serial.println("ESP-NOW inicialitzat correctament");

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, recieverAddress, 6);
  peerInfo.channel = 6;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Error trobant receptor");
    return;
  }

  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  delay(10);

  if (!bme280.begin(0x76)) {
    Serial.println("No s'ha trobat cap sensor BME280 vàlid");
    while (true) { delay(1000); }
  }

  esp_sleep_enable_timer_wakeup(5000000);
}

void loop() {
  dades.pressio = bme280.readPressure() / 100;
  dades.temperatura = bme280.readTemperature();
  dades.humitat = bme280.readHumidity();

  dataSent = false;
  esp_now_send(recieverAddress, (uint8_t *)&dades, sizeof(dades));

  unsigned long startTime = millis();
  while (!dataSent && millis() - startTime < 500) {
    delay(10);
  }

  Serial.println("Entrant a mode baix consum...");
  Serial.flush();

  digitalWrite(4, LOW);

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  esp_deep_sleep_start();
}