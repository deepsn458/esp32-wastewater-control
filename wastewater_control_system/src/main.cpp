#include <Arduino.h>
#include <Wire.h>
#include <Ezo_i2c.h>
#include <Ezo_i2c_util.h>
#include "i2cSensors.h"


void setup() {
  TaskHandle_t sensorMonitoringTask_handle= NULL;
  xTaskCreatePinnedToCore(sensorMonitoringTask,"Sensor Monitoring",2048, NULL, 1, &sensorMonitoringTask_handle, 1);
}

void loop() {
  
}