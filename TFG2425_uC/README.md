# ESP32 Smart Home

Este repositorio contiene el firmware de varios nodos ESP32 interconectados usando ESP-NOW y Firebase. Cada nodo tiene una función específica dentro de una red domótica.

## Estructura del proyecto

- `BME280_reading`: lee los valores del sensor BME280 y los envía por ESP-NOW.
- `PIR_Rele`: recibe datos del PIR y activa un relé.
- `LEDs_RGB_vFB`: controla una tira de LEDs RGB mediante comandos de Firebase.
- `ESpCentral_FBcontrol`: actúa como gateway central, recibe datos de sensores y los envía a Firebase.

## Requisitos

- Placas ESP32
- Sensores utilizados
- Bibliotecas necesarias
- Conexión Wi-Fi con acceso a internet
- Cuenta en Firebase

## Instalación

1. Clona este repositorio:
   ```bash
   git clone https://github.com/3r3p/TFG2425_uC.git
   ```

2. Abre cada carpeta con el IDE de Arduino o PlatformIO.

3. Configura los siguientes valores en cada `.ino`:
   - SSID y password de Wi-Fi
   - API Key y URL de Firebase
   - MACs de los dispositivos

## Licencia

Este proyecto está licenciado bajo la [MIT License](LICENSE).
