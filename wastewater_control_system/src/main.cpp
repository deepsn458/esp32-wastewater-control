#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseClient.h>
#include "database.h"
#include <Ezo_i2c.h>
#include "tankControl.h"
#include "sensorMonitoring.h"
#include "electrodialysisControl.h"
#include <Wire.h>

#define PRESSURE_LIMIT 100
const int PG01_SENSOR_ADDRESS = 114;
const int LLS04LowPin = 14;
const int LLS04HighPin = 35;
TaskHandle_t monitorSensorsHandle = NULL;
TaskHandle_t controlPH01Handle = NULL;
TaskHandle_t controlPH02Handle = NULL;
Ezo_board pG01Sensor = Ezo_board(PG01_SENSOR_ADDRESS, "pG01");
/* Using core 1 of ESP32 */
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif


#define WIFI_SSID "Rice Visitor"

const char* DATABASE_URL = "https://waste-water-control-default-rtdb.firebaseio.com/";

void setup(){
  pinMode(LLS04HighPin, INPUT_PULLUP);
  pinMode(LLS04LowPin, INPUT_PULLUP);
  Wire.begin();
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print(WiFi.localIP());
  initFirebase(DATABASE_URL);
  Serial.println("Firebase is ready");
  xTaskCreatePinnedToCore(controlPH01,"PH01 Control", 6048,NULL,2, &controlPH01Handle, app_cpu);
  xTaskCreatePinnedToCore(controlPH02,"PH02 Control", 6048,NULL,2, &controlPH02Handle, app_cpu);
}



void loop() {
  // Keep Firebase tasks running
  fireBaseLoop();
  //pushSensorReading("Pressure", "PG02",89);
  delay(100); // minor delay
}


void monitorLLS04(){
  if (!digitalRead(LLS04HighPin)){
    Serial.println("Tank 8 high level alert");
  }
  else if (digitalRead(LLS04LowPin)){
    Serial.println("Tank 8 low level alert");
  }
}

void monitorPG01(){
  pG01Sensor.send_read_cmd();
  delay(1000);
  pG01Sensor.receive_read_cmd();
  if (pG01Sensor.get_last_received_reading() > PRESSURE_LIMIT){
    Serial.println("PG01 is above 100psi");
  }
  else{
    Serial.println("PG01 is normal");
  }
}
