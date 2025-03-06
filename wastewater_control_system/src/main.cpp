/*In this basic free RTOS tutorial will use xTaskCreatePinnedToCore() . 
in order to enable us to select which ESP32 core (core 0 or core 1) will run the particular task.*/

#include <Arduino.h>
#include <Wire.h>
#include "tank1Control.h"
#include "sensorMonitoring.h"
#include "electrodialysisControl.h"

TaskHandle_t controlTank01Handle = NULL; 
TaskHandle_t monitorSensorsHandle = NULL;
TaskHandle_t controlElectrodialysisHandle = NULL;

//===========================================================================================//
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
 
  Serial.println("Hello, ESP32 with FreeRTOS !");
  xTaskCreatePinnedToCore (controlTank01, "Tank01 Control", 2048, NULL, 1, &controlTank01Handle, 1);
  xTaskCreatePinnedToCore (monitorSensors, "sensor Monitoring", 2048, NULL, 1, &monitorSensorsHandle, 1);
  xTaskCreatePinnedToCore (controlElectrodialysis, "sensor Monitoring", 2048, NULL, 2, &controlElectrodialysisHandle, 1);
}

void loop() {
 

}
