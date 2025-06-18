#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

const uint8_t PIN_RELE = 13;
const uint8_t PIN_PIR = 12;

bool presenciaPIR = false;
esp_now_peer_info_t peerInfo;
uint8_t recieverAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; // REEMPLAZAR amb la MAC del receptor

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Estat d'enviament: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Missatge enviat correctament" : "Error enviant missatge");
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_RELE, OUTPUT);
  pinMode(PIN_PIR, INPUT);
  digitalWrite(PIN_RELE, HIGH);
  delay(100);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE);
  Serial.println("Canal WiFi canviat correctament. Nou canal WiFi: 6");

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
  delay(60000);
  Serial.println("Sensor PIR inicialitzat");
  Serial.println("------------------------------------------------");
  Serial.println("------------------------------------------------");
  Serial.println(" ");
}

void loop() {
  presenciaPIR = digitalRead(PIN_PIR);
  esp_now_send(recieverAddress, (uint8_t *)&presenciaPIR, sizeof(presenciaPIR));

  if (presenciaPIR) {
    digitalWrite(PIN_RELE, LOW);
    Serial.println("Presència detectada");
    delay(120000);
  } else {
    digitalWrite(PIN_RELE, HIGH);
    Serial.println("No s'ha detecta res");
    delay(1000);
  }
}
