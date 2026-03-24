#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// =========================
// الإعدادات
// =========================
#define DHTPIN D2
#define DHTTYPE DHT22          // إذا كان الحساس DHT11 غيّرها إلى DHT11

#define GAS_DIGITAL_PIN D5
#define GAS_ANALOG_PIN A0

#define BUZZER_PIN D6

const int LED_PIN = LED_BUILTIN;
const bool LED_ACTIVE_HIGH = false;

// إنذار الحرارة
const float TEMPERATURE_ALERT_THRESHOLD = 35.0;

// إذا كان البزر يعمل عند HIGH اتركها true
// إذا اشتغل بالعكس غيّرها إلى false
const bool BUZZER_ACTIVE_HIGH = false;

// =========================
// الكائنات والمتغيرات
// =========================
ESP8266WebServer server(80);
DHT dht(DHTPIN, DHTTYPE);

bool ledState = false;
bool buzzerState = false;

float lastTemperature = NAN;
float lastHumidity = NAN;
int lastGasDigital = HIGH;
int lastGasAnalog = 0;

// =========================
// وظائف مساعدة
// =========================
void setLed(bool state) {
  ledState = state;

  if (LED_ACTIVE_HIGH) {
    digitalWrite(LED_PIN, state ? HIGH : LOW);    
  } else {
    digitalWrite(LED_PIN, state ? LOW : HIGH); 
     }
}

void setBuzzer(bool state) {
  buzzerState = state;

  if (BUZZER_ACTIVE_HIGH) {
    digitalWrite(BUZZER_PIN, state ? HIGH : LOW);
  } else {
    digitalWrite(BUZZER_PIN, state ? LOW : HIGH);
  }
}

void readDhtSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (!isnan(h)) {
    lastHumidity = h;
  }

  if (!isnan(t)) {
    lastTemperature = t;
  }
}

void readGasSensor() {
  lastGasDigital = digitalRead(GAS_DIGITAL_PIN);
  lastGasAnalog = analogRead(GAS_ANALOG_PIN);
}

bool isGasDetected() {
  return lastGasDigital == LOW;
}

bool isTemperatureAlert() {
  if (isnan(lastTemperature)) {
    return false;
  }
  return lastTemperature >= TEMPERATURE_ALERT_THRESHOLD;
}

//bool shouldAlarm() {
 // return isGasDetected() || isTemperatureAlert() || ledState;
//}
bool shouldAlarm() {

  // إذا كان الضوء يعمل
  if (ledState) {
    return true;
  }

  // إذا اكتشف غاز
  if (isGasDetected()) {
    return true;
  }

  // إذا الحرارة وصلت 35
  if (isTemperatureAlert()) {
    return true;
  }

  // شرط جديد: إذا الحرارة ≥ 25
  if (!isnan(lastTemperature) && lastTemperature >= 25.5) {
    return true;
  }

  return false;
}
String getGasStatusText() {
  if (isGasDetected()) {
    return "detected";
  }
  return "normal";
}

String getGasStatusArabic() {
  if (isGasDetected()) {
    return "تم اكتشاف غاز / دخان";
  }
  return "طبيعي";
}

String getTemperatureAlertArabic() {
  if (isTemperatureAlert()) {
    return "درجة الحرارة مرتفعة";
  }
  return "طبيعية";
}

void updateAlarmOutput() {
  readDhtSensor();
  readGasSensor();

  if (shouldAlarm()) {
    setBuzzer(true);
  } else {
    setBuzzer(false);
  }
}

String jsonStatus() {
  updateAlarmOutput();

  String json = "{";
  json += "\"device\":\"NodeMCU ESP8266\",";
  json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
  json += "\"led\":" + String(ledState ? "true" : "false") + ",";
  json += "\"led_status\":\"" + String(ledState ? "on" : "off") + "\",";

  if (isnan(lastTemperature)) {
    json += "\"temperature\":null,";
  } else {
    json += "\"temperature\":" + String(lastTemperature, 1) + ",";
  }

  if (isnan(lastHumidity)) {
    json += "\"humidity\":null,";
  } else {
    json += "\"humidity\":" + String(lastHumidity, 1) + ",";
  }

  json += "\"temperature_alert\":" + String(isTemperatureAlert() ? "true" : "false") + ",";
  json += "\"temperature_threshold\":" + String(TEMPERATURE_ALERT_THRESHOLD, 1) + ",";

  json += "\"gas_digital\":" + String(isGasDetected() ? "true" : "false") + ",";
  json += "\"gas_digital_raw\":" + String(lastGasDigital) + ",";
  json += "\"gas_analog\":" + String(lastGasAnalog) + ",";
  json += "\"gas_status\":\"" + getGasStatusText() + "\",";

  json += "\"buzzer\":" + String(buzzerState ? "true" : "false");
  json += "}";

  return json;
}

// =========================
// صفحات الويب
// =========================
void handleRoot() {
  updateAlarmOutput();

  String tempText = isnan(lastTemperature) ? "غير متوفر" : String(lastTemperature, 1) + " °C";
  String humText = isnan(lastHumidity) ? "غير متوفر" : String(lastHumidity, 1) + " %";
  String gasDigitalText = getGasStatusArabic();
  String gasAnalogText = String(lastGasAnalog);
  String tempAlertText = getTemperatureAlertArabic();
  String buzzerText = buzzerState ? "يعمل" : "متوقف";

  String html =
    "<!DOCTYPE html>"
    "<html lang='ar' dir='rtl'>"
    "<head>"
    "<meta charset='UTF-8'>"
    "<meta name='viewport' content='width=device-width, initial-scale=1'>"
    "<title>NodeMCU Sensors</title>"
    "<style>"
    "body{font-family:Tahoma,Arial,sans-serif;background:#111827;color:#fff;padding:20px;margin:0;}"
    ".box{max-width:760px;margin:30px auto;background:#1f2937;padding:20px;border-radius:16px;box-shadow:0 10px 30px rgba(0,0,0,.25);}"
    "h1{margin-top:0;}"
    ".info{background:#374151;padding:14px;border-radius:10px;margin:10px 0;font-size:18px;}"
    "a{display:inline-block;margin:8px;padding:12px 18px;border-radius:10px;text-decoration:none;color:#fff;}"
    ".on{background:#16a34a;}"
    ".off{background:#dc2626;}"
    ".api{background:#2563eb;}"
    ".warn{background:#7c2d12;}"
    ".ok{background:#065f46;}"
    "</style>"
    "<meta http-equiv='refresh' content='3'>"
    "</head>"
    "<body>"
    "<div class='box'>"
    "<h1>لوحة تحكم NodeMCU ESP8266</h1>"
    "<div class='info'><strong>IP:</strong> " + WiFi.localIP().toString() + "</div>"
    "<div class='info'><strong>حالة الإضاءة:</strong> " + String(ledState ? "ON" : "OFF") + "</div>"
    "<div class='info'><strong>درجة الحرارة:</strong> " + tempText + "</div>"
    "<div class='info'><strong>الرطوبة:</strong> " + humText + "</div>"
    "<div class='info'><strong>تنبيه الحرارة:</strong> " + tempAlertText + "</div>"
    "<div class='info'><strong>حالة الغاز الرقمية:</strong> " + gasDigitalText + "</div>"
    "<div class='info'><strong>قراءة الغاز الأنالوج:</strong> " + gasAnalogText + "</div>"
    "<div class='info'><strong>حالة البزر:</strong> " + buzzerText + "</div>";

  if (isGasDetected()) {
    html += "<div class='info warn'><strong>تنبيه غاز:</strong> تم رصد غاز أو دخان</div>";
  }

  if (isTemperatureAlert()) {
    html += "<div class='info warn'><strong>تنبيه حرارة:</strong> درجة الحرارة وصلت أو تجاوزت " + String(TEMPERATURE_ALERT_THRESHOLD, 1) + " °C</div>";
  }

  if (!shouldAlarm()) {
    html += "<div class='info ok'><strong>الحالة:</strong> كل شيء طبيعي</div>";
  }

  html +=
    "<a class='on' href='/led/on'>تشغيل LED</a>"
    "<a class='off' href='/led/off'>إطفاء LED</a>"
    "<a class='api' href='/api/status'>API Status</a>"
    "</div>"
    "</body>"
    "</html>";

  server.send(200, "text/html; charset=UTF-8", html);
}

void handleLedOn() {
  setLed(true);
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleLedOff() {
  setLed(false);
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleApiStatus() {
  server.send(200, "application/json", jsonStatus());
}

void handleApiLedOn() {
  setLed(true);
  server.send(200, "application/json", jsonStatus());
}

void handleApiLedOff() {
  setLed(false);
  server.send(200, "application/json", jsonStatus());
}

void handleApiDht() {
  readDhtSensor();

  String json = "{";

  if (isnan(lastTemperature)) {
    json += "\"temperature\":null,";
  } else {
    json += "\"temperature\":" + String(lastTemperature, 1) + ",";
  }

  if (isnan(lastHumidity)) {
    json += "\"humidity\":null,";
  } else {
    json += "\"humidity\":" + String(lastHumidity, 1) + ",";
  }

  json += "\"temperature_alert\":" + String(isTemperatureAlert() ? "true" : "false") + ",";
  json += "\"temperature_threshold\":" + String(TEMPERATURE_ALERT_THRESHOLD, 1);
  json += "}";

  server.send(200, "application/json", json);
}

void handleApiGas() {
  readGasSensor();

  String json = "{";
  json += "\"gas_digital\":" + String(isGasDetected() ? "true" : "false") + ",";
  json += "\"gas_digital_raw\":" + String(lastGasDigital) + ",";
  json += "\"gas_analog\":" + String(lastGasAnalog) + ",";
  json += "\"gas_status\":\"" + getGasStatusText() + "\"";
  json += "}";

  server.send(200, "application/json", json);
}

void handleApiBuzzerOn() {
  setBuzzer(true);
  server.send(200, "application/json", "{\"buzzer\":true}");
}

void handleApiBuzzerOff() {
  setBuzzer(false);
  server.send(200, "application/json", "{\"buzzer\":false}");
}

void handleNotFound() {
  String json = "{";
  json += "\"error\":\"not found\",";
  json += "\"path\":\"" + server.uri() + "\"";
  json += "}";
  server.send(404, "application/json", json);
}

// =========================
// setup / loop
// =========================
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  setLed(false);

  pinMode(BUZZER_PIN, OUTPUT);
  setBuzzer(false);

  pinMode(GAS_DIGITAL_PIN, INPUT);

  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println();
  Serial.println("Connecting WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/led/on", HTTP_GET, handleLedOn);
  server.on("/led/off", HTTP_GET, handleLedOff);

  server.on("/api/status", HTTP_GET, handleApiStatus);
  server.on("/api/led/on", HTTP_GET, handleApiLedOn);
  server.on("/api/led/off", HTTP_GET, handleApiLedOff);
  server.on("/api/dht", HTTP_GET, handleApiDht);
  server.on("/api/gas", HTTP_GET, handleApiGas);
  server.on("/api/buzzer/on", HTTP_GET, handleApiBuzzerOn);
  server.on("/api/buzzer/off", HTTP_GET, handleApiBuzzerOff);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();
  updateAlarmOutput();
  delay(100);
}