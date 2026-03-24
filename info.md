Web Endpoints
Main Page
/
LED Control
/led/on
/led/off
API
/api/status
/api/led/on
/api/led/off
/api/dht
/api/gas
/api/buzzer/on
/api/buzzer/off
Alarm Logic

The buzzer can be configured to work when:

Gas is detected
Temperature reaches the configured threshold
LED is turned on
Any custom condition you add in the code
Notes
MQ gas sensors may need some warm-up time after power-on.
DHT11/DHT22 should be powered from 3V3.
MQ sensor is powered from VIN.
If the buzzer logic is inverted, change the buzzer logic setting in code.
Future Improvements
Add different buzzer patterns for gas and temperature alerts
Add OLED display support
Add MQTT support
Add Telegram or email alerts
Save readings to a database


NodeMCU

D2  → DHT DATA
3V3 → DHT VCC
GND → DHT GND

D5  → MQ DO
A0  → MQ AO
VIN → MQ VCC
GND → MQ GND

D6  → BUZZER +
GND → BUZZER -

LED → LED_BUILTIN
