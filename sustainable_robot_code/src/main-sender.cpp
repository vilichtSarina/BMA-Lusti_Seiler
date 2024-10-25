#include <esp_now.h>
#include <WiFi.h>

// ESP RECEIVER'S MAC ADDRESS
uint8_t broadcastAddress[] = {0x58, 0xBF, 0x25, 0x93, 0xEE, 0x18};

typedef struct joystick_data {
  int x;
  int y;
} joystick_data;

joystick_data joystickData;

esp_now_peer_info_t peerInfo;

//Joystick
const int X_pin = 34; // analog input pin for x
const int Y_pin = 35; // analog input pin for y

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(9600);
 
  WiFi.mode(WIFI_STA);
 
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_send_cb(OnDataSent);
   
  // register peer
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  // register receiver esp 
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  joystickData.x = analogRead(X_pin);
  joystickData.y = analogRead(Y_pin);
  Serial.println("x:" + joystickData.x);
  Serial.println("y:" + joystickData.y);
 
  esp_err_t result = esp_now_send(0, (uint8_t *) &joystickData, sizeof(joystick_data));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(2000);
}
