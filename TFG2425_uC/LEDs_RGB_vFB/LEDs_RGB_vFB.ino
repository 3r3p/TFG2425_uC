#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <FastLED.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Credenciales Wi-Fi (REEMPLAZAR)
#define WIFI_SSID "TU_SSID"
#define WIFI_PASSWORD "TU_PASSWORD"

// Configuración Firebase (REEMPLAZAR)
#define API_KEY "TU_API_KEY"
#define DATABASE_URL "https://TU_PROJECT.firebaseio.com/"

// Configuración Neopixel
#define CTRL_PIN 18
#define NUM_LEDS 30

CRGB leds[NUM_LEDS];

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

bool signupOK = false;
uint8_t r = 0, g = 0, b = 0;
uint8_t hue = 0;

void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connectant al Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nWi-Fi connectat");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    signupOK = true;
    Serial.println("SignUP a Firebase realitzat correctament");
  } else {
    Serial.printf("Error de sign-up: %s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  FastLED.addLeds<WS2812B, CTRL_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();
}

void loop() {
  if (Firebase.RTDB.getString(&fbdo, "/special")) {
    if (fbdo.stringData() != "NS") {
      Serial.println("Arcoiriiiiiiis!!");
      rainbowEffect(leds, NUM_LEDS, hue);
    } else {
      if (Firebase.RTDB.getInt(&fbdo, "/color/R")) {
        r = fbdo.intData();
      }

      if (Firebase.RTDB.getInt(&fbdo, "/color/G")) {
        g = fbdo.intData();
      }

      if (Firebase.RTDB.getInt(&fbdo, "/color/B")) {
        b = fbdo.intData();
      }

      Serial.print("R: ");
      Serial.println(r);
      Serial.print("G: ");
      Serial.println(g);
      Serial.print("B: ");
      Serial.println(b);
      Serial.println("---------------------------------");

      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(r, g, b);
      }
    }
  }
  FastLED.show();
  delay(250);
}

void rainbowEffect(CRGB* leds, int numLeds, uint8_t& hue) {
  for (int i = 0; i < numLeds; i++) {
    leds[i] = CHSV(hue + i * 5, 255, 255);
  }
  hue++;
}
