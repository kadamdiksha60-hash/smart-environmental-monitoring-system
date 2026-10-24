
#include "DHT.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define DHTPIN 15
#define DHTTYPE DHT22

const char* ssid = "Wokwi-GUEST";
const char* password = "";

// ThingSpeak Write API Key
const char* apiKey = "YOUR_THINGSPEAK_WRITE_API_KEY";

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected!");
}


void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check for sensor errors
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("ERROR: Failed to read DHT sensor!");
    delay(2000);
    return;
  }

  Serial.println("\n===== SMART ENVIRONMENT MONITOR =====");

  // Display temperature
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  // Display humidity
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Check alert conditions
  bool highTemperature = temperature > 30.0;
  bool highHumidity = humidity > 70.0;

  if (highTemperature || highHumidity) {
    Serial.println("Status: ALERT");

    if (highTemperature) {
      Serial.println("Warning: HIGH TEMPERATURE");
    }

    if (highHumidity) {
      Serial.println("Warning: HIGH HUMIDITY");
    }

  } else {
    Serial.println("Status: NORMAL");
    Serial.println("Message: Environment within limits");
  }

  // Send data to ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = "http://api.thingspeak.com/update?api_key=";
    url += apiKey;
    url += "&field1=";
    url += String(temperature, 2);
    url += "&field2=";
    url += String(humidity, 2);

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String response = http.getString();
      int entryID = response.toInt();

      Serial.print("ThingSpeak HTTP Code: ");
      Serial.println(httpCode);

      Serial.print("ThingSpeak Entry ID: ");
      Serial.println(entryID);

      if (httpCode == 200 && entryID > 0) {
        Serial.println("SUCCESS: Data uploaded to ThingSpeak!");
      } else {
        Serial.println(
          "WARNING: Data not saved. Check API key or update interval."
        );
      }

    } else {
      Serial.print("ERROR: HTTP request failed: ");
      Serial.println(http.errorToString(httpCode));
    }

    http.end();

  } else {
    Serial.println("ERROR: WiFi disconnected!");
  }

  Serial.println("====================================");


  delay(20000);
}
