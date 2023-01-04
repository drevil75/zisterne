/* Arduino example sketch to control a JSN-SR04T ultrasonic distance sensor with Arduino. No library needed. More info: https://www.makerguides.com */

// Define Trig and Echo pin:
#define trigPin 5
#define echoPin 4

// Define variables:
long duration;
int distance;

void setup() {
  // Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Begin Serial communication at a baudrate of 9600:
  Serial.begin(9600);
}

void loop() {
  
  //---------------Sensor-----------------------------------
  digitalWrite(trigPin, LOW); // Clear the trigPin by setting it LOW:
  delayMicroseconds(10);
  digitalWrite(trigPin, HIGH);  // Trigger the sensor by setting the trigPin high for 10 microseconds:
  delayMicroseconds(20);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds:
  distance = duration*0.034/2;  // Calculate the distance:
  //---------------Sensor-----------------------------------
  
  delay(2000);
}