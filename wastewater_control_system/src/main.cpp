#include <Arduino.h>
#include <Ezo_i2c.h>
#include "i2cSensors.h"
#include "rs485Devices.h"

/* Using core 1 of ESP32 */
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif
void setup() {
  //wait for start signal
  /*TODO: Implement Start signal*/

  TaskHandle_t controlPumps_handle = NULL;
  TaskHandle_t sensorMonitoringTask_handle = NULL;
  xTaskCreatePinnedToCore (sensorMonitoringTask, "Sensor Monitoring", 2048, NULL, 1, &sensorMonitoringTask_handle, app_cpu);
  xTaskCreatePinnedToCore (controlPumps, "Control Pumps", 2048, NULL, 1, &controlPumps_handle, app_cpu);
}

void loop() {
  // put your main code here, to run repeatedly:
}

