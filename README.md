# NodeMCU ESP8266 Sensor Web Server

مشروع باستخدام **NodeMCU ESP8266** لقراءة:

- **درجة الحرارة والرطوبة** عبر حساس **DHT11 / DHT22**
- **الغاز / الدخان** عبر حساس **MQ**
- **البزر** للتنبيه
- **LED** للتحكم اليدوي

ويعرض كل ذلك من خلال **صفحة ويب** و **API JSON**.

---

## Features

- الاتصال بشبكة Wi-Fi
- Web Server على ESP8266
- قراءة درجة الحرارة والرطوبة
- قراءة حساس الغاز الرقمي والأنالوج
- تشغيل البزر عند:
  - اكتشاف الغاز
  - وصول الحرارة إلى الحد المطلوب
  - تشغيل الضوء
- التحكم في LED من صفحة الويب
- API endpoints لقراءة البيانات

---

## Hardware Used

- NodeMCU ESP8266
- DHT11 أو DHT22
- MQ Gas Sensor
- Buzzer
- Jumper Wires
- Breadboard (optional)

---

## Pin Connections

### DHT Sensor
- `VCC -> 3V3`
- `GND -> GND`
- `DATA -> D2`

### MQ Gas Sensor
- `VCC -> VIN`
- `GND -> GND`
- `DO -> D5`
- `AO -> A0`

### Buzzer
- `SIG / + -> D6`
- `GND / - -> GND`

### LED
- Built-in LED on the NodeMCU board (`LED_BUILTIN`)

---

## PlatformIO Configuration

Example `platformio.ini`:

```ini
[env:nodemcuv2]
platform = espressif8266
board = nodemcuv2
framework = arduino

upload_speed = 115200
monitor_speed = 115200

lib_deps =
  adafruit/DHT sensor library
  adafruit/Adafruit Unified Sensor
