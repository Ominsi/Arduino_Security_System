#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
/*
 * Security System
 * Kienen Mulloy
 * ESP Board
 */
#include <WiFiClient.h>

WiFiClient client;
HTTPClient http;

String EMAIL = "EMAIL"; // Email for alert to be sent to.
String HTTP_URL = "API URL"; // API URL

void setup() {
  Serial.begin(9600); // Start Serial

  WiFi.begin("WIFI NAME","WIFI PASSWORD"); //Connect to Wifi
  
  // Wait for connection to be made.
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println();
  Serial.println(WiFi.localIP()); // Print the IP Address connected to

  http.begin(client, HTTP_URL+EMAIL); // Begin http with WifiClient and url of the API URL and email concatinated.
  
}
void loop() {
  // When serial is available, read from it and if it is true then send post request to the HTTPClient.
  if (Serial.available()) {
    boolean receive = Serial.read();
    if (receive) {
      http.POST(httpRequestData);
    }
  }

}
