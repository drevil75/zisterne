#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";


String request = "http://192.168.0.50:18080/rest/items/txtrest/state";
HTTPClient http;
http.begin(request);
http.addHeader("Accept", "application/json");
http.addHeader("Content-Type", "text/plain");
int httpResponseCode = http.POST("Hello, World!");
