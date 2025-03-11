#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
// Home
// const char *ssid = "353-1Atwood";
// const char *password = "Natezacdan1";
// Hotspot
const char *ssid = "Daniel’s iPhone (5)";
const char *password = "maya12345";

// MQTT Broker
// Home
// const char *mqtt_broker = "10.0.0.52";
// Hotspot
const char *mqtt_broker = "172.20.10.6";

const char *topic = "sensor";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
    // Set software serial baud to 115200
    Serial.begin(115200);
    // Connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the Wi-Fi network");
    // Connecting to the MQTT broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "esp32-client";
        Serial.printf("\nThe client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str())) {
            Serial.println("Public EMQX MQTT broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
    // Publish and subscribe
    // client.publish(topic, "Hi, I'm ESP32");
    // client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void loop() {
    client.loop();
    // Generate a random norm value
    float voltageValue = ((float)rand() / RAND_MAX) * 3.3; // Generate a random voltage between 0 and 3.3V
    // Convert the voltage value to a string
    char voltageStr[8];
    dtostrf(voltageValue, 1, 2, voltageStr);
    // Publish the voltage value to the MQTT broker
    client.publish(topic, voltageStr);
    // Wait for 20 milliseconds (50 readings per second)
    delay(100);
}
