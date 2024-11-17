#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <esp_now.h>

#include "ESC.h"

const int kJoystickMin = 0;
const int kJoystickMax = 4095;

// Pin definition for ESCs, which control the BLDCs.
const int kEscLeft = 25;
const int kEscRight = 26;

// Used for steering.
const int kServoPinRight = 12;
const int kServoPinLeft = 13;

// Represents X and Y coordinates of a given joystick position. Both axes have
// values ranging from 0 to 4096.
typedef struct JoystickData {
  int x = 0;
  int y = 0;
  int z = 0;

  void print() { printf("x:%d, y:%d", x, y); }
} JoystickData;

JoystickData joystickData;

ESC escRight(kEscRight, /*minimum=*/1000, /*maximum=*/2000, /*arm=*/500);
ESC escLeft(kEscLeft, /*minimum=*/1000, /*maximum=*/2000, /*arm=*/500);

Servo servoRight;
Servo servoLeft;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&joystickData, incomingData, sizeof(joystickData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  joystickData.print();
}

// Used to execute arming command, which is needed to start the motors.
void arm() {
  escRight.arm();
  escLeft.arm();
}

void adjustAngle(const int angle) {
  servoLeft.write(angle);
  servoRight.write(angle);
}

void adjustSpeed(const int speed) {
  escLeft.speed(speed);
  escRight.speed(speed);
}

void setup() {
  Serial.begin(9600);

  // Init ESP-NOW for communication.
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  servoRight.attach(kServoPinRight);
  servoLeft.attach(kServoPinLeft);
}

void loop() {
  // If joystick button is pressed down, execute the arming command needed to
  // init the BLDCs.
  if (joystickData.z == LOW) {
    arm();
  }

  const int angle = map(joystickData.x, kJoystickMin, kJoystickMax, -90, 90);
  adjustAngle(angle);

  const int speed =
      map(joystickData.y, kJoystickMin, kJoystickMax, 0, /*maximum=*/2000);
  adjustSpeed(speed);
}