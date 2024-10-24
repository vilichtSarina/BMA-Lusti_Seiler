#include <Arduino.h>
#include <ESP32Servo.h>

// Pin definition for misc. actuators.
const int kButton = 15;
const int kLight = 26;

// Pin definition for joystick.
const int kXpin = 32;
const int kYpin = 33;

Servo servo;
int servoPin = 25;
int pos = 0;

int current_state = HIGH;
int lastPostition = 90;

void setup() {
  Serial.begin(9600);
  servo.attach(servoPin, 800, 2200);
}

void loop() {
  servo.write(90);
  delay(1000);

  Serial.printf("X: %d\t\t", analogRead(kXpin));
  Serial.print("\n");
}