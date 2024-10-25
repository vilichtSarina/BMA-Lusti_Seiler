#include <Arduino.h>
#include <ESP32Servo.h>

// Pin definition for misc. actuators.
const int kButton = 15;
const int kLight = 26;

// Pin definition for joystick.
const int kXpin = 32;
const int kYpin = 33;

// Pin definition for 1 channel relay.
const int kRelayR = 32;
const int kRelayL = 33;

Servo servoR;
int servoPinR = 26;
Servo servoL;
const int servoPinL = 25;

int currentState = HIGH;
int lastPostition = 90;

void setup() {
  Serial.begin(9600);
  pinMode(servoPinR, OUTPUT);
  servoR.attach(servoPinR, 800, 2200);
  pinMode(kRelayR, OUTPUT);

  pinMode(servoPinL, OUTPUT);
  servoL.attach(servoPinL, 800, 2200);
  pinMode(kRelayL, OUTPUT);
}

// int activationState() {}

void loop() {
  servoR.write(-90);
  servoL.write(90);

  if (currentState == HIGH) {
    digitalWrite(kRelayR, HIGH);
    digitalWrite(servoPinR, HIGH);

    digitalWrite(kRelayL, HIGH);
    digitalWrite(servoPinL, HIGH);

    currentState = LOW;

    delay(3000);

  } else {
    digitalWrite(kRelayR, LOW);
    digitalWrite(servoPinR, LOW);

    digitalWrite(kRelayL, LOW);
    digitalWrite(servoPinL, LOW);

    currentState = HIGH;
  }
  delay(1000);
}