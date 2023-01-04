//-----------------WIFI-Daten----------------
const char* ssid = "MeinWIFI-Name";         //WLan-Name
const char* password = "MeinWIFIpasswort";  //WLAN-Passwort
//-------------------------------------------


#include <ESP8266WiFi.h>
#include <math.h>

// Define Trig and Echo pin:
#define trigPin 5
#define echoPin 4

// Define variables:
long duration;
float distance;
int level;
float lastval;
float distmax = 200; //maximaler Abstand zur Wasseroberfläche - Zisterne leer
float distmin = 20; //minimaler Abstand zur Wasseroberfläche - Zisterne voll
float dist;
float proz;


WiFiServer server(80);



void setup() {
  Serial.begin(115200);
  delay(10);

  //-----------Sensor-------------------
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //-----------Sensor-------------------

  //-----------WIFI-------------------
  // Mit Wifi verbinden
  Serial.print("Verbinden mit: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi verbunden");

  // Start des Servers
  server.begin();
  Serial.println("Server gestartet");

  // Print the IP address
  Serial.print("Diese URL zum Verbinden aufrufen: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  //-----------WIFI-------------------

}

void loop() {

  //---------------Sensor-----------------------------------
  digitalWrite(trigPin, LOW); // Clear the trigPin by setting it LOW:
  delay(5);
  digitalWrite(trigPin, HIGH);  // Trigger the sensor by setting the trigPin high for 10 microseconds:
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds:
  distance = duration*0.034/2;  // Calculate the distance:

  //manchmal kommen vom Sensor ungültige Werte zurück.
  if (distance >= distmin && distance <= distmax) {

      if (distance <= distmin) {
        dist = 1; //Division durch 0 verhindern
      }
      else {
        dist = distance;
      }

    proz = 100 - ((dist - distmin) / (distmax - distmin) * 100);
    level = round(proz);
    lastval = level;
  }
  else {
    level = lastval;
  }


  Serial.print("duration=");
  Serial.println(duration);
  Serial.print("distance=");
  Serial.println(distance);
  Serial.print("level=");
  Serial.println(level);
  //---------------Sensor-----------------------------------


  //---------------Server-----------------------------------
  // Prüfen, ob sich ein Client verbunden hat

  // Anfrage zurücksenden
  // Return the response
  WiFiClient client = server.available();
    if (!client) {
    return;
    }
    else {
      measure();
    }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Access-Control-Allow-Origin: *");
  client.println("");

  client.print("{");
  client.print("\"level\":");
  client.print(level);
  client.print("}");
  //---------------Server-----------------------------------
  delay(3000);
}
