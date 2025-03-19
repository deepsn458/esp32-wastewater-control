#include <Arduino.h>
#include <Wire.h>
#include <Ezo_i2c.h>
#include <Ezo_i2c_util.h>
#include "i2cSensors.h"
#include "dataLogging.h"
#include "rs485Devices.h"
#include "acidPump.h"

#define RXD2 16
#define TXD2 17
HardwareSerial sensor(2);

void setup() {
  Serial.begin(115200);
  sensor.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Wire.begin();
  TaskHandle_t sensorMonitoringTask_handle= NULL;
  TaskHandle_t dataLogging_handle = NULL;
  TaskHandle_t controlPumps_handle = NULL;
  TaskHandle_t doseAcid_handle = NULL;
  //xTaskCreatePinnedToCore(sensorMonitoringTask,"Sensor Monitoring",2048, NULL, 1, &sensorMonitoringTask_handle, 1);
  //xTaskCreatePinnedToCore(controlPumps, "Data Logging", 4096, NULL, 1, &controlPumps_handle, 1);
  //xTaskCreatePinnedToCore(doseAcid, "Dose Acid", 2048, NULL, 2,&doseAcid_handle,1);

}

void loop() {
  sensor.print("I2C,109");
  sensor.print("\r");
  if (sensor.available()>0){
    Serial.println(sensor.read());
  }
    
}