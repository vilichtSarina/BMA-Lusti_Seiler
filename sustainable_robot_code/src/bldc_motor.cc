#include <Arduino.h>
#include <ESP32Servo.h>

#include "ESC.h"

#define ESC_PIN (33)
#define LED (2)  // Build-in LED, must be configured for ESP32.
#define JOYSTICK_PIN (34)

#define MIN_SPEED 1040  // Lower speed can turn off the motor.
#define MAX_SPEED 1240

ESC myESC(ESC_PIN, 1000, 2000, 500);

long int joystick_value;

void setup() {
  // Baud rate.
  Serial.begin(9600);
  delay(1000);

  pinMode(JOYSTICK_PIN, INPUT);
  pinMode(ESC_PIN, OUTPUT);
  pinMode(LED, OUTPUT);

  // Send arming command to the ESC.
  myESC.arm();
  // Arming complete will make the buildin LED shine.
  digitalWrite(LED, HIGH);
  delay(5000);

  // the following loop turns on the motor slowly, so get ready
  for (int i = 0; i < 350; i++) {  // run speed from 840 to 1190
    myESC.speed(MIN_SPEED - 200 +
                i);  // motor starts up about half way through loop
    delay(10);
  }
}  // speed will now jump to pot setting

void loop() {
  // When connected to steering, joystick value will be between 0 and 4095.
  // These values should be mapped to the PWM duty cycle min and max of the BLDC
  // motor.
  // TODO: remove hard-coded value, set up connection with joystick modules.
  joystick_value = 2000;
  Serial.println(joystick_value);
  joystick_value = map(joystick_value, 0, 4095, MIN_SPEED, MAX_SPEED);

  // Set speed to the mapped value.
  myESC.speed(joystick_value);
  // Give motor 10ms to reach the speed.
  delay(10);
}
