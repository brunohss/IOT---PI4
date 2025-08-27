// ESP32 -> MQTT -> Telemetry JSON example
// Libraries: PubSubClient, ArduinoJson, WiFi, LiquidCrystal_I2C

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---- WiFi & MQTT ----
const char* WIFI_SSID = "ClaroVirtua170_2G";
const char* WIFI_PASS = "48608741";
const char* MQTT_HOST = "192.168.0.133"; // e.g., 192.168.0.10
const uint16_t MQTT_PORT = 1883;
const char* TOPIC = "lab/bench/bench01/telemetry";

WiFiClient espClient;
PubSubClient mqtt(espClient);

unsigned long lastPub = 0;

// ====== Sensores (ESP32-S3) ======
#define DHTPIN   17
#define DHTTYPE  DHT11
DHT dht(DHTPIN, DHTTYPE);

#define LDR_PIN    2   // ADC1_CH1
#define KY038_PIN  1   // ADC1_CH2

// ===== LCD 20x4 I2C ======
LiquidCrystal_I2C lcd(0x27, 20, 4); // Endereço 0x27 comum, ajuste se necessário

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Inicializa LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Iniciando...");

  // Configura ADC
  analogReadResolution(12);           // 0..4095
  analogSetAttenuation(ADC_11db);     // ~0.15V a ~3.3V

  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  lcd.setCursor(0,1);
  lcd.print("Conectando WiFi");
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  lcd.setCursor(0,1);
  lcd.print("WiFi conectado     ");

  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  reconnectMQTT();
}

void reconnectMQTT() {
  lcd.setCursor(0,2);
  lcd.print("MQTT: Conectando  ");
  while (!mqtt.connected()) {
    Serial.print("Connecting MQTT...");
    if (mqtt.connect("esp32-bench01")) {
      Serial.println("connected");
      lcd.setCursor(0,2);
      lcd.print("MQTT conectado     ");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" retrying in 2s");
      lcd.setCursor(0,2);
      lcd.print("MQTT failed        ");
      delay(2000);
    }
  }
  lcd.setCursor(0, 2);       // vai para o início da linha 2
  lcd.print("                    "); // imprime 20 espaços (para LCD 20x4)
}

void loop() {
  // ===== Leituras =====
  float hum  = dht.readHumidity();
  float temp = dht.readTemperature();
  int   lux = analogRead(LDR_PIN);
  int   noise_db = analogRead(KY038_PIN);

  // Valida DHT
  if (isnan(hum) || isnan(temp)) {
    Serial.println("Falha na leitura do DHT11");
    lcd.setCursor(0,0);
    lcd.print("Erro DHT11        ");
  } else {
    Serial.printf("Temp: %.1f C  Hum: %.0f %%\n", temp, hum);
    lcd.setCursor(0,0);
    lcd.printf("Temp: %.1fC Umi: %.0f%%", temp, hum);
  }

  Serial.printf("LDR:%d  SOM:%d\n", lux, noise_db);
  lcd.setCursor(0,1);
  lcd.printf("LUX:%4d Ruido:%4d   ", lux, noise_db);

  // Atualiza status WiFi/MQTT
  lcd.setCursor(0,3);
  lcd.printf("WiFi:%s MQTT:%s", WiFi.status()==WL_CONNECTED?"OK":"OFF", mqtt.connected()?"OK":"OFF");

  if (!mqtt.connected()) reconnectMQTT();
  mqtt.loop();

  unsigned long now = millis();
  if (now - lastPub > 2000) { // publish every 2s
    lastPub = now;

    StaticJsonDocument<256> doc;
    doc["bench_id"]   = "bench01";
    doc["temp"]       = temp;
    doc["hum"]        = hum;
    doc["lux"]        = lux;
    doc["noise_db"]   = noise_db;
    doc["ts"]         = (uint32_t)(millis()/1000);

    char payload[256];
    size_t n = serializeJson(doc, payload);
    mqtt.publish(TOPIC, payload, n);
    Serial.println(payload);

    lcd.setCursor(16,3);
    lcd.printf("Send");


  }
}
