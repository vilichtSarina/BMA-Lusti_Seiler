#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include "ESC.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/soc.h"

const int kJoystickMin = 0;
const int kJoystickMax = 4095;
const int kHighThreshold = 3500;
const int kLowThreshold = 500;

// Pin definition for ESCs, which control the BLDCs.
const int kEscLeft = 25;
const int kEscRight = 26;

// Builtin LED to signal arming.
const int kLED = 2;

// Used for steering.
const int kServoPinRight = 12;
const int kServoPinLeft = 13;

// Represents X and Y coordinates of a given joystick position. Both axes have
// values ranging from 0 to 4096.
typedef struct JoystickData {
  int xDir;
  int xSpeed;
  int ySpeed;

  void print() { printf("x:%d, xS:%d, yS:%d\n", xDir, xSpeed, ySpeed); }
} JoystickData;

JoystickData joystickData;

ESC escRight(kEscRight, /*minimum=*/1000, /*maximum=*/2000, /*arm=*/500);
ESC escLeft(kEscLeft, /*minimum=*/1000, /*maximum=*/2000, /*arm=*/500);

Servo servoRight;
Servo servoLeft;

enum class Direction { kRight, kLeft, kNone };
enum class Speed { kForward, kNone };

Direction direction = Direction::kNone;
Speed lastSpeed = Speed::kNone;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&joystickData, incomingData, sizeof(joystickData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  joystickData.print();
}

// Used to execute arming command, which is needed to start the motors.
void arm() {
  digitalWrite(kLED, HIGH);
  escRight.arm();
  escLeft.arm();
  digitalWrite(kLED, LOW);
}

Speed GetSpeed() {
  if (joystickData.xSpeed >= kHighThreshold) {
    return Speed::kForward;
  }
  return Speed::kNone;
}

// Given the joystick data, sets the servo direction.
Direction GetDirection() {
  if (joystickData.xDir >= kHighThreshold && GetSpeed() == Speed::kNone) {
    return Direction::kRight;
  }
  if (joystickData.xDir <= kLowThreshold && GetSpeed() == Speed::kNone) {
    return Direction::kLeft;
  }
  return Direction::kNone;
}

void maybeAttach() {
  if (!servoRight.attached()) {
    servoRight.attach(kServoPinRight);
  }
  if (!servoLeft.attached()) {
    servoLeft.attach(kServoPinLeft);
  }
}

void SetServos(Direction direction) {
  switch (direction) {
    case Direction::kRight:
      maybeAttach();
      servoRight.write(30);
      servoLeft.write(30);
      break;
    case Direction::kLeft:
      maybeAttach();
      servoRight.write(180);
      servoLeft.write(180);
      break;
  }
  servoRight.detach();
  servoLeft.detach();

  // Serial.println("Neutral");
}

void SetSpeed(Speed currentSpeed) {
  if (currentSpeed == Speed::kForward && lastSpeed == Speed::kNone) {
    // Serial.print("on");
    for (int i = 0; i < 350; i++) {  // run speed from 840 to 1190
      escLeft.speed(1115 - 200 + i);
      escRight.speed(1115 - 200 + i);
      delay(10);
    }
  } else if (lastSpeed == Speed::kForward && currentSpeed == Speed::kNone) {
    escLeft.stop();
    escLeft.speed(0);
    escRight.stop();
    escRight.speed(0);
  }
  lastSpeed = currentSpeed;
}

void setup() {
  Serial.begin(9600);
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  // Init ESP-NOW for communication.
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  servoRight.attach(kServoPinRight);
  servoLeft.attach(kServoPinLeft);

  pinMode(kLED, OUTPUT);
}

void loop() {
  // If joystick button is pressed down, execute the arming command needed to
  // init the BLDCs.
  if (joystickData.ySpeed == LOW) {
    arm();
  }

  SetServos(GetDirection());
  SetSpeed(GetSpeed());
}