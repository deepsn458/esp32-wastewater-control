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

//UART Serial Pins
#define RXD2 16
#define TXD2 17

#define PRESSURE_LIMIT 100


TaskHandle_t monitorSensorsHandle = NULL;
TaskHandle_t controlPH01Handle = NULL;
TaskHandle_t controlPH02Handle = NULL;
Ezo_board pHSensor_adjustment = Ezo_board(PH_SENSOR_ADDRESS_ADJUSTMENT, "PH_ADJ");
#define DOSING_PUMP_ADDRESS_1 0x39  // Example address for Tank 1 dosing pump
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
  Wire.begin();
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print(WiFi.localIP());
  initFirebase(DATABASE_URL);
  xTaskCreatePinnedToCore(phControl,"PH Control", 10000,NULL,2, &controlPH02Handle, app_cpu);
  xTaskCreatePinnedToCore(monitorSensors,"Monitor Sensors", 10000, NULL, 2,&monitorSensorsHandle, app_cpu);
  
  //Cleans the database before each run
  deleteData();
}



void loop() {
  // Keep Firebase tasks running
  fireBaseLoop();
  
  delay(100); // minor delay
  
}


void phControl(void* params){
 
  TickType_t lastDoseTick = 0;
  for (;;){
    pHSensor_adjustment.send_read_cmd();
    vTaskDelay(1000/portTICK_PERIOD_MS);
    pHSensor_adjustment.receive_read_cmd();
    float pHReading = pHSensor_adjustment.get_last_received_reading();
    uint32_t dosingDelaySeconds = max(30.0,(120-(pHReading-7.5)*60));
    TickType_t dosingDelayTicks = pdMS_TO_TICKS(dosingDelaySeconds*1000);
    Serial.println(pHReading);
    pushSensorReading("ph",pHSensor_adjustment.get_name(), pHSensor_adjustment.get_last_received_reading());
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

