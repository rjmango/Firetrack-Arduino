#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h> // For creating JSON payload

#define RXPin D6   // GPS TX → NodeMCU D6 (GPIO12) [Connect Tx pin of GPS Module to D6 Pin of NodeMCU]
#define TXPin D5   // GPS RX → NodeMCU D5 (GPIO14) [Connect Rx pin of GPS Module to D5 Pin of NodeMCU]
#define GPSBaud 9600

const char* ssid = "***********";     // Enter your wifi ssid
const char* password = "*********";   // Enter you wifi password
// const char* serverHost = "192.168.1.9"; // Replace with your server's IP or hostname
// const int serverPort = 8080;                 // If using HTTP on port 80, you can omit this
// const String apiPath = "/api/map/coordinates/"; // Your API endpoint for POST requests
const String deviceId = "ESP_" + String(ESP.getChipId());

float lat = 0, lng = 0;

// Data Sending Interval
unsigned long previousMillis = 0;
const long interval = 5000; // Send location every 5 seconds

SoftwareSerial gpsSerial(RXPin, TXPin);
TinyGPSPlus gps;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);
  gpsSerial.begin(GPSBaud);
  WiFi.begin(ssid, password);

  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  while (gpsSerial.available()) {
    char c = gpsSerial.read();
    gps.encode(c);
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis < interval) continue; 
    
  previousMillis = currentMillis;

  // Guard clause for checking WiFi connection before sending
  if (WiFi.status() != WL_CONNECTED) { 
    Serial.println("WiFi disconnected. Data not sent.");
    // optionally try to reconnect here.
    WiFi.begin(ssid, password);
    continue;
  } 

  sendCoordinate();
}

void sendCoordinate() {
  // Check if gps data valid
  if (!gps.location.isValid()) {
    Serial.println("Invalid GPS location. Data not sent.");
    return;
  }

  float latitude = gps.location.lat();
  float longitude = gps.location.lng();

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  // String serverUrl = String("http://") + serverHost + ":" + String(serverPort) + apiPath;
  String serverUrl = String("https://fireapi.maxellmilay.com/api/map/coordinates/");
  Serial.print("WiFi Status: ");
  Serial.println(serverUrl);

  // Check if http client connected
  if (!http.begin(client, serverUrl)){
    Serial.printf("[HTTP} Unable to connect\n");
    http.end();
    return;
  }

  Serial.print("[HTTP] POST...\n");

  // Create JSON payload
  StaticJsonDocument<256> jsonDocument;
  jsonDocument["tracker_id"] = deviceId;
  jsonDocument["latitude"] = latitude;
  jsonDocument["longitude"] = longitude;
  
  String jsonString;
  serializeJson(jsonDocument, jsonString);

  // Set headers
  http.addHeader("Content-Type", "application/json");

  // Send HTTP POST request
  int httpCode = http.POST(jsonString);


  if (httpCode <= 0) {
    Serial.printf("2[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    http.end();
    return;
  }
  
  Serial.printf("[HTTP] POST... code: %d\n", httpCode);

  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED || httpCode == HTTP_CODE_NO_CONTENT) {
    String payload = http.getString();
    Serial.println("received payload:");
    Serial.println(payload);

    // Flash Built in LED if POST Successful
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    Serial.printf("1[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}