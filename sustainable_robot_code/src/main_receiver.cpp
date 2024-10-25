#include <Arduino.h>
#include <ESP32Servo.h>
#include <esp_now.h>
#include <WiFi.h>

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

typedef struct joystick_data {
  int x;
  int y;
} joystick_data;

joystick_data joystickData;

//callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&joystickData, incomingData, sizeof(joystickData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("x: ");
  Serial.println(joystickData.x);
  Serial.print("y: ");
  Serial.println(joystickData.y);
  Serial.println();
}

void setup() {
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);

  //Init ESP-NOW for communication
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

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