#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>  // Use Secure Client for TLS
#include <DHT.h>
#include "dataLogging.h"

WiFiClientSecure wifiClient;  // Use Secure Client
PubSubClient client(wifiClient);

// WiFi setup
const char *ssid = "Wokwi-GUEST";
const char *password = "";

// MQTT Broker setup
const char* mqtt_broker = "dcf9b2457e7b4ac58c487ffe3ed9af34.s1.eu.hivemq.cloud";
const char* topic = "sensorData";
const char* mqtt_username = "test1";
const char* mqtt_password = "Test!123";
const char* client_ID = "ESP32_client";
const int mqtt_port = 8883;

// DHT sensor setup
const int DHTPIN = 12;
const int DHTTYPE = 22;
DHT dht(DHTPIN, DHTTYPE);

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
        if (client.connect(client_ID, mqtt_username, mqtt_password)) {
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
    dht.begin();

    // Connecting to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to Wi-Fi");

    // Enable TLS (skip certificate validation)
    wifiClient.setInsecure();

    // Connect to the MQTT broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);

    for (;;) {
        if (!client.connected()) {
            reconnect();
        }
        client.loop();  // Ensure MQTT stays connected

        float temp = dht.readTemperature();
        if (!isnan(temp)) {
            String msgStr = "Temperature: " + String(temp);
            client.publish(topic, msgStr.c_str());
            Serial.println("Published: " + msgStr);
        } else {
            Serial.println("Failed to read temperature from DHT sensor!");
        }

        vTaskDelay(6000 / portTICK_PERIOD_MS);
    }
}
