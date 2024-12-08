#include "DHT.h"
#define DHTPIN 0
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#include <Servo.h>  // Include the Servo library
// wiper
Servo wiperServo;  // Create a Servo object

// ping sensor
const int trigPin = 14;  // Trigger pin
const int echoPin = 12;  // Echo pin

// weather led
int relay_1 = 16;
int relay_2 = 5;
// system power
int relay_3 = 13;

// raindrop sensor
int raindrop_pin = 4;

// cloudy sensor
int ldr_pin = A0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();
  pinMode(relay_1, OUTPUT);
  pinMode(relay_2, OUTPUT);
  pinMode(relay_3, OUTPUT);
  wiperServo.attach(2);  // Attach the servo to pin 2
  // pinMode(btn_pin, INPUT);
  pinMode(raindrop_pin, INPUT);
  pinMode(ldr_pin, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(relay_3, HIGH);
}

float distance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the duration of the echo
  long duration = pulseIn(echoPin, HIGH);

  // Calculate distance in centimeters
  // Speed of sound = 343 m/s (34,300 cm/s)
  // Distance = (Duration / 2) * (Speed of Sound)
  float distanceCm = (duration / 2.0) * 0.0343;
  return distanceCm;
}

void loop() {
  // put your main code here, to run repeatedly:
  // int btn = digitalRead(btn_pin);
  float t = dht.readTemperature();
  Serial.print("Temp: ");
  Serial.println(t);

  int raindrop = digitalRead(raindrop_pin);
  Serial.print("raindrop: ");
  Serial.println(raindrop);

  int ldr = analogRead(ldr_pin);
  Serial.print("ldr: ");
  Serial.println(ldr);

  float distanceCm = distance();
  Serial.print("Distance: ");
  Serial.print(distanceCm);
  Serial.println(" cm");

  if (t >= 30) {
    Serial.println("Suhu terlalu panas!");
    Serial.print("sistem di hentikan sementara hingga suhu normal ");
    Serial.print("(suhu saat ini: ");
    Serial.print(t);
    Serial.println("°C)");
    for (int i = 1; i < 6; i++) {
      Serial.print("Waiting ");
      Serial.print(i);
      Serial.println(" Second");
      delay(1000);
    }
    digitalWrite(relay_3, LOW);  // system off
    while (t >= 30) {
      t = dht.readTemperature();
      raindrop = digitalRead(raindrop_pin);
      ldr = analogRead(ldr_pin);
      distanceCm = distance();

      Serial.print("suhu saat ini: ");
      Serial.print(t);
      Serial.println("°C");
      // raindrop
      Serial.print("raindrop: ");
      Serial.println(raindrop);
      // ldr
      Serial.print("ldr: ");
      Serial.println(ldr);
      // ping
      Serial.print("Distance: ");
      Serial.print(distanceCm);
      Serial.println(" cm");

      delay(500);
    }
    digitalWrite(relay_3, HIGH);  // system on
    raindrop = 1;
  } else {
    if (raindrop == LOW) {
      digitalWrite(relay_2, HIGH);
      Serial.println("HUJAN");
      wiperServo.write(180);  // Move to 180°
      delay(500);             // Pause for 1 second
      wiperServo.write(0);    // Move to 0°
      delay(500);             // Pause for 1 second at 0°
    } else if (ldr < 20 && raindrop == HIGH) {
      digitalWrite(relay_2, LOW);
      digitalWrite(relay_1, HIGH);
      Serial.println("MENDUNG");
    } else {
      digitalWrite(relay_2, LOW);
      digitalWrite(relay_1, LOW);
    }
  }
  delay(500);
}
