#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;

// WiFi
// Home
// const char *ssid = "353-1Atwood";
// const char *password = "Natezacdan1";
// Hotspot
const char* ssid = "Daniel’s iPhone (5)";
const char* password = "maya12345";

// MQTT Broker
// Home
// const char *mqtt_broker = "10.0.0.52";
// Hotspot
const char *mqtt_broker = "172.20.10.6";

const char *topic = "sensor";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// ADC configuration constants
const float ADC_RESOLUTION = 65535.0; // 16-bit resolution (2^16 - 1)
const float ADC_VOLTAGE_MAX = 3.3;    // Maximum voltage (adjust according to reference voltage)

void setup() {
    // Set software serial baud to 115200
    Serial.begin(115200);
    
    if (!ads.begin()) {
      Serial.println("Failed to initialize ADS.");
      while (1);
    }
    
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

// Function to convert raw ADC reading to voltage
float convertToVoltage(int16_t rawReading) {
    return (rawReading / ADC_RESOLUTION) * ADC_VOLTAGE_MAX;
}

void loop() {
    client.loop();
    
    int16_t adc0;
    float volts0;

    adc0 = ads.readADC_SingleEnded(0);
    // adc1 = ads.readADC_SingleEnded(1);
    // adc2 = ads.readADC_SingleEnded(2);
    // adc3 = ads.readADC_SingleEnded(3);

    volts0 = ads.computeVolts(adc0);
    // volts1 = ads.computeVolts(adc1);
    // volts2 = ads.computeVolts(adc2);
    // volts3 = ads.computeVolts(adc3);
    
    // Convert the voltage value to a string
    char voltageStr[8];
    dtostrf(volts0, 1, 2, voltageStr);
    
    // Publish the voltage value to the MQTT broker
    client.publish(topic, voltageStr);
    
    // Wait for 100 milliseconds
    delay(100);
}
