#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>  // Use Secure Client for TLS
#include "dataLogging.h"

WiFiClientSecure wifiClient;  // Use Secure Client
PubSubClient client(wifiClient);

// WiFi setup
const char *ssid = "Rice Visitor";
//change if connecting to anetwork with a password
const char *password = "";


// MQTT Broker setup
const char* mqtt_broker = "dcf9b2457e7b4ac58c487ffe3ed9af34.s1.eu.hivemq.cloud";
const char* topic = "sensorData";
const char* client_ID  = "test1";
const char* client_password = "Neodynium!123";
const int mqtt_port = 1883;


// MQTT callback function
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message has arrived in: ");
    Serial.println(topic);
    Serial.print("Message: ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

// Reconnect function
void reconnect() {
    while (!client.connected()) {
        if (client.connect(client_ID)) {
            Serial.println("MQTT Connected");
            client.subscribe(topic);
        } else {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.println(" Retrying in 5 seconds...");
            delay(5000);
        }
    }
}

// Data logging task
void dataLogging(void* parameters) {
    Serial.begin(115200);

    // Connecting to Wi-Fi (add passwod field if necessat)
    WiFi.begin(ssid);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    Serial.print("Connected to");Serial.println(WiFi.localIP());
    // Connect to the MQTT broker
    
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    
    client.subscribe("new_topic");
    for (;;) {
        if (!client.connected()) {
            reconnect();
        }
        client.loop();  // Ensure MQTT stays connected
        client.publish("new_topic","hi");
        Serial.println("hi");
        

        vTaskDelay(6000 / portTICK_PERIOD_MS);
    }
}
