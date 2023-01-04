//-----------------WIFI-Daten----------------
const char* ssid = "MeinWIFI-Name";         //WLan-Name
const char* password = "MeinWIFIpasswort";  //WLAN-Passwort
//-------------------------------------------


//-----------Zisternen-Daten-----------------
float distmin = 10;                       //minimaler Abstand vom Sensor zur Wasseroberfläche - Zisterne ist voll
float distmax = 185;                      //maximaler Abstand vom Sensor zur Wasseroberfläche - Zisterne ist leer

//  _____ _ Deckel
//  # s # - Sensor
//  #   #
// ##~~~## - distmin - Wasseroberfläche
// #~~~~~#
// #~~~~~#
// #~~~~~#
// ####### - distmax - Grund
//-------------------------------------------

//-----------Chip-Pin-Belegung---------------
#define trigPin 5 //TX / D1         //GPIO für den Trigger-PIN
#define echoPin 4 //RX / D2         //GPIO für den Receiver-PIN
//-------------------------------------------

//-----------Ab hier sollte nichts mehr verändert werden------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
long duration;
float distance;
int level;
float lastval;
float dist;
float proz;


#include <ESP8266WiFi.h>
#include <math.h>
//#include <ESP8266HTTPClient.h>
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
  client.println();
  client.println("<!DOCTYPE html>");
  client.println("<html xmlns='http://www.w3.org/1999/xhtml'>");
  client.println("<meta http-equiv='content-type' content='text/html; charset=utf-8'>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
  client.println("<head><title>Wasserstand Regenwasserzisterne</title></head>");
  client.println("<body>");
  client.println("<h1>Zisterne</h1>");
  client.println("<h2>Die Zisterne ist zu " + String(level) + "% gefüllt.</h2>");
  client.println("<progress max='100' value='" + String(level) + "' style='width:300px;height:200px'></progress>");
  client.println("</body></html>");

  //---------------Server-----------------------------------
  delay(3000);
}
