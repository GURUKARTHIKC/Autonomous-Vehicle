#include <esp_now.h>
#include <WiFi.h>

// Structure to send data
typedef struct struct_message {
    char a[32];
    int b;
    float c;
    bool d;
} struct_message;

// Create a struct_message to send data
struct_message myData;

// Callback function executed when data is received
void OnDataRecv(const esp_now_recv_info * info, const uint8_t *incomingData, int len) {
    memcpy(&myData, incomingData, sizeof(myData));
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Char: ");
    Serial.println(myData.a);
    Serial.print("Int: ");
    Serial.println(myData.b);
    Serial.print("Float: ");
    Serial.println(myData.c);
    Serial.print("Bool: ");
    Serial.println(myData.d);
    Serial.println();
}

// Callback function executed when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);
    
    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
    
    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Register callback functions
    esp_now_register_recv_cb(OnDataRecv);
    esp_now_register_send_cb(OnDataSent);

    // Register the peer (Board 2)
    esp_now_peer_info_t peerInfo;
    uint8_t peerAddress[] = {0x10, 0x06, 0x1C, 0x82, 0x0F, 0xCC}; // Replace with Board 2's MAC address
    memcpy(peerInfo.peer_addr, peerAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    
    // Add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
    }
}

void loop() {
    // Set values to send
    strcpy(myData.a, "Hello from Board 2");
    myData.b = random(1, 20);
    myData.c = 1.2;
    myData.d = false;
    
    // Send data to Board 2
    uint8_t peerAddress[] = {0x10 ,0x06 ,0x1C ,0x82 ,0x0F ,0xCC}; // Replace with Board 1's MAC address
    esp_err_t result = esp_now_send(peerAddress, (uint8_t *) &myData, sizeof(myData));
    
    if (result == ESP_OK) {
        Serial.println("Sent with success");
    } else {
        Serial.println("Error sending the data");
    }
    
  delay(2000);
}
