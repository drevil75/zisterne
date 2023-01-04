//-----------------WIFI-Daten----------------
const char* ssid = "MeinWIFI-Name";         //WLan-Name
const char* password = "MeinWIFIpasswort";  //WLAN-Passwort
//-------------------------------------------

//-----------Openhab-Daten-------------------
String ohaddress = "192.168.0.50:8080";  //IP-Adresse:Port oder DNS-Name:Port
String itemname_lvl = "WasserZisterne";          //hier bei Bedarf den Namen des Items für den Füllstand in % eintragen.
long refreshRate = 60;                     //Interval in Sekunden, wie oft der Wasserstand an den Server übermittelt wird.
//-------------------------------------------

//-----------Zisternen-Daten-----------------
float distmin = 41;                       //minimaler Abstand vom Sensor zur Wasseroberfläche - Zisterne ist voll
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
String request = String("http://") + String(ohaddress) + String("/rest/items/") + String(itemname_lvl);
long duration;
float distance;
float disttmp1;
float disttmp2;
float disttmp3;
int level;
float lastval;
float dist;
float proz;


#include <ESP8266WiFi.h>
#include <math.h>
#include <ESP8266HTTPClient.h>


void setup() {
  Serial.begin(115200);
  delay(10);

  //-----------Sensor-------------------
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //-----------Sensor-------------------

  //-----------WIFI-------------------
  // Mit Wifi verbinden
  Serial.print("WIFI wird verbunden");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi verbunden");

  // Print the IP address
  Serial.print("Der ESP8266 hat die IP-Adresse ");
  Serial.println(WiFi.localIP());

  //-----------WIFI-------------------

}

void loop() {

  //---------------Sensor-----------------------------------
  distance = getDistance();
  //sometimes the sensor returns invalid values. only valid values will processed.
  if (distance >= 20 && distance <= 500) { //Der Messbereich des Sensors liegt laut Hersteller zwischen 20 und 500cm.

      if (distance <= distmin) {
        dist = 1; //Division durch 0 verhindern
      }
      else {
        dist = distance;
      }

    proz = 100 - ((dist - distmin) / (distmax - distmin) * 100); //Füllstand berechnen
    level = round(proz); //Füllstand auf ganze Zahlen runden.

    //Werte < 0 und > 100 abschneiden. Kann bei kleineren Ungenauigkeiten bei der Messung oder der der Erfassung der Zisternenmaße distmin/distmax vorkommen.
    if (level < 0) {level = 0;}
    if (level > 100) {level = 100;}

    postValue(level); //nur valide Werte an den Server posten.
    Serial.print("Messung an den Server gepostet");
  }
  else {
    Serial.print("Invalide Messung. Es wird nichts an den Server gepostet");
  }

  Serial.print("duration=");
  Serial.println(duration);
  Serial.print("distance=");
  Serial.println(distance);
  Serial.print("level=");
  Serial.println(level);
  //---------------Sensor-----------------------------------



  long timer = long(refreshRate * 1000);
  Serial.println(timer);
  delay(timer);

}

float getDistance() {

  //3x messen und anschließend den Mittelwert bilden. Soll Ungenauigkeiten in der Messung herausmitteln.

  //erste Messung
  digitalWrite(trigPin, LOW); // Clear the trigPin by setting it LOW:
  delay(5);
  digitalWrite(trigPin, HIGH);  // Trigger the sensor by setting the trigPin high for 10 microseconds:
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds:
  disttmp1 = duration*0.034/2;  // Calculate the distance:
  delay(1000);

  //zweite Messung
  digitalWrite(trigPin, LOW); // Clear the trigPin by setting it LOW:
  delay(5);
  digitalWrite(trigPin, HIGH);  // Trigger the sensor by setting the trigPin high for 10 microseconds:
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds:
  disttmp2 = duration*0.034/2;  // Calculate the distance:
  delay(1000);

  //dritte Messung
  digitalWrite(trigPin, LOW); // Clear the trigPin by setting it LOW:
  delay(5);
  digitalWrite(trigPin, HIGH);  // Trigger the sensor by setting the trigPin high for 10 microseconds:
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds:
  disttmp3 = duration*0.034/2;  // Calculate the distance:

  distance = float((disttmp1 + disttmp2 + disttmp3) / 3);
  return distance;
  }

int postValue(int val) {
  //---------------Server-----------------------------------
  // POST-Request an den Openhab-Server senden

  HTTPClient http;
  http.addHeader("Accept", "application/json");
  http.addHeader("Content-Type", "text/plain");
  http.begin(request);
  int httpResponseCode = http.POST(String(val));
  Serial.println(request);
  Serial.println(httpResponseCode);
  http.end();   //Close connection
  return httpResponseCode;
  //---------------Server-----------------------------------
}
