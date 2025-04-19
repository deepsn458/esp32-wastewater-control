#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseClient.h>
#include "database.h"
#include <Ezo_i2c.h>
#include "tankControl.h"
#include "sensorMonitoring.h"
#include "electrodialysisControl.h"
#include <Wire.h>
#include <algorithm>

#define PRESSURE_LIMIT 100
const int PG01_SENSOR_ADDRESS = 114;
const int LLS04LowPin = 14;
const int LLS04HighPin = 35;
TaskHandle_t monitorSensorsHandle = NULL;
TaskHandle_t controlPH01Handle = NULL;
TaskHandle_t controlPH02Handle = NULL;
Ezo_board pG01Sensor = Ezo_board(PG01_SENSOR_ADDRESS, "pG01");
Ezo_board pHSensor = Ezo_board(99, "pH02");
#define DOSING_PUMP_ADDRESS_1 0x38  // Example address for Tank 1 dosing pump
Ezo_board dosingPumpTank01 = Ezo_board(DOSING_PUMP_ADDRESS_1, "PMP1");
/* Using core 1 of ESP32 */
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

void phControl(void* params);
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
  //xTaskCreatePinnedToCore(controlPH01,"PH01 Control", 6048,NULL,2, &controlPH01Handle, app_cpu);
  //xTaskCreatePinnedToCore(controlPH02,"PH02 Control", 6048,NULL,2, &controlPH02Handle, app_cpu);
  xTaskCreatePinnedToCore(monitorSensors,"Monitor Sensors", 8048, NULL, 2,&monitorSensorsHandle, app_cpu);
}



void loop() {
  // Keep Firebase tasks running
  fireBaseLoop();
  //pushSensorReading("Pressure", "PG02",89);
  //pushSensorReading("PH",pHSensor.get_name(), pHSensor.get_last_received_reading());
  delay(100); // minor delay
}

void phControl(void* params){
 
  TickType_t lastDoseTick = 0;
  for (;;){
    pHSensor.send_read_cmd();
    vTaskDelay(1000/portTICK_PERIOD_MS);
    pHSensor.receive_read_cmd();
    float pHReading = pHSensor.get_last_received_reading();
    uint32_t dosingDelaySeconds = max(30.0,(120-(pHReading-7.5)*60));
    TickType_t dosingDelayTicks = pdMS_TO_TICKS(dosingDelaySeconds*1000);
    Serial.println(pHReading);
    pushSensorReading("PH",pHSensor.get_name(), pHSensor.get_last_received_reading());
  if (pHReading > 7.5){
    Serial.println(xTaskGetTickCount() - lastDoseTick);
    if ((xTaskGetTickCount() - lastDoseTick) >= dosingDelayTicks) {
        dosingPumpTank01.send_cmd_with_num("d,", 0.5);
        lastDoseTick = xTaskGetTickCount();
        Serial.println("pumping");
    }
  }
  }
  
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
