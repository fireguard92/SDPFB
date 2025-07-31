#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <TinyGPSPlus.h>

#define MQ7_PIN 35
#define MQ2_PIN 34

const char* ssid = "UNIPOD-STUDIO-1 2304";
const char* password = "737J@22j";
const char* scriptURL = "https://script.google.com/macros/s/AKfycbyGtcSDeIb9QFWYFDsfTgQLtM10TPQHCSTnXte3ONyC9E1-VLQHd_IhZWSHI6VqG5xprQ/exec";

Adafruit_BME280 bme;
TinyGPSPlus gps;

HardwareSerial GPSSerial(2);  // UART2 : RX=16, TX=17

void setup() {
  Serial.begin(115200);
  GPSSerial.begin(9600, SERIAL_8N1, 16, 17); // RX, TX du GPS

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connecté");

  if (!bme.begin(0x76)) {
    Serial.println("BME280 non détecté !");
    while (1);
  }

  pinMode(MQ7_PIN, INPUT);
  pinMode(MQ2_PIN, INPUT);
}

void loop() {
  // Lecture GPS pendant quelques millisecondes
  while (GPSSerial.available()) {
    gps.encode(GPSSerial.read());
  }

  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  int mq7_raw = analogRead(MQ7_PIN);
  int mq2_raw = analogRead(MQ2_PIN);

  String gpsPosition = "N/A";
  if (gps.location.isValid()) {
    gpsPosition = String(gps.location.lat(), 6) + "," + String(gps.location.lng(), 6);
  }

  Serial.println("---- Données capteurs ----");
  Serial.print("Température : "); Serial.println(temperature);
  Serial.print("Humidité : "); Serial.println(humidity);
  Serial.print("MQ7 : "); Serial.println(mq7_raw);
  Serial.print("MQ2 : "); Serial.println(mq2_raw);
  Serial.print("GPS : "); Serial.println(gpsPosition);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(scriptURL);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"temperature\":" + String(temperature) +
                     ",\"humidity\":" + String(humidity) +
                     ",\"co\":" + String(mq7_raw) +
                     ",\"gas\":" + String(mq2_raw) +
                     ",\"gps\":\"" + gpsPosition + "\"}";

    int code = http.POST(payload);
    Serial.print("HTTP code : ");
    Serial.println(code);
    Serial.println("Réponse : " + http.getString());

    http.end();
  }

  delay(10000);
}
