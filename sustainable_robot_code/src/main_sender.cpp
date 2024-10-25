#include <WiFi.h>
#include <esp_now.h>

namespace {
// Overwriting library type definitions for clean code outside of anonymous
// namespace.
using EspNowPeerInfo = esp_now_peer_info_t;
using EspErr = esp_err_t;
using EspNowSend = esp_now_send_cb_t;
using EspNowSendStatus = esp_now_send_status_t;

EspErr EspNowAddPeer(const esp_now_peer_info_t *peer) {
  return esp_now_add_peer(peer);
}

EspErr EspNowSendData(const uint8_t *peer_addr, const uint8_t *data,
                      size_t len) {
  return esp_now_send(peer_addr, data, len);
}

EspErr EspNowRegisterAndSendCb(EspNowSend cb) {
  return esp_now_register_send_cb(cb);
}

EspErr EspNowInit() { return esp_now_init(); }

}  // namespace

// The hexadecimal representation of the ESP receiver's mac address.
const uint8_t RECEIVER_BROADCAST_ADDR[] = {0x58, 0xBF, 0x25, 0x93, 0xEE, 0x18};
// The pin constants for the joystick axes.
const int X_PIN = 34;
const int Y_PIN = 35;

// Represents X and Y coordinates of a given joystick position. Both axes have
// values ranging from 0 to 4096.
typedef struct JoystickData {
  int x;
  int y;

  void print() { printf("x:%d, y:%d", x, y); }
} JoystickData;

// Callback function to pass to EspNowPeerInfo when messages are sent. Returns
// status of the messaging.
void OnDataSent(const uint8_t *mac_addr, EspNowSendStatus status) {
  // A mac string always contains 18 characters due to the
  char mac[18];

  Serial.print("Packet to: ");
  // Copies the sender mac address to a string.
  snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0],
           mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.printf("%s send status:\t", mac);
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success"
                                                : "Delivery Fail");
}

// Initialize and setup GPIO pins and establish WiFi connection for
// communication between ESPs.
void setup() {
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);

  const EspErr init_status = EspNowInit();
  if (init_status != ESP_OK) {
    Serial.printf("Error initializing ESP-NOW with error %d.\n", init_status);
    return;
  }

  EspNowRegisterAndSendCb(OnDataSent);

  // Register the peer_info by copying the hard-coded broadcast address used to
  // connect.
  EspNowPeerInfo peer_info{.channel = 0, .encrypt = false};
  memcpy(peer_info.peer_addr, RECEIVER_BROADCAST_ADDR, 6);

  // Register actual hardware using the previously initialized peer_info.
  const EspErr esp_add_status = EspNowAddPeer(&peer_info);
  if (esp_add_status != ESP_OK) {
    Serial.printf("Failed to add peer with error %d\n.", esp_add_status);
    return;
  }
}

void loop() {
  JoystickData joystick_data{.x = analogRead(X_PIN), .y = analogRead(Y_PIN)};
  joystick_data.print();

  EspErr send_data_status =
      EspNowSendData(0, (uint8_t *)&joystick_data, sizeof(JoystickData));

  if (send_data_status == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.printf("Failed sending data with error %d.\n", send_data_status);
  }

  delay(2000);
}
