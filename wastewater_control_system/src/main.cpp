#include <Arduino.h>
#include <Ezo_i2c.h>
#include "tankControl.h"
#include "sensorMonitoring.h"
#include "electrodialysisControl.h"

#define PRESSURE_LIMIT 100

Ezo_board pG01Sensor = Ezo_board(PG01_SENSOR_ADDRESS, "pG01");
/* Using core 1 of ESP32 */
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

void setupHardware();
void setupNetwork();

void controlPG01(void* parameters);

void setup() {
  // set up all the sensors and drivers and the network
  setupHardware();
  setupNetwork();

  //wait for start signal
  /*TODO: Implement Start signal*/

  TaskHandle_t controlTank01_handle = NULL;
  TaskHandle_t monitorSensors_handle = NULL;
  TaskHandle_t controlPG01_handle = NULL;
  TaskHandle_t controlElectrodialysis_handle = NULL;
  xTaskCreatePinnedToCore (controlTank01, "Tank01 Monitoring", 2048, NULL, 1, &controlTank01_handle, app_cpu);
  xTaskCreatePinnedToCore (monitorSensors, "Sensor Monitoring", 2048, NULL, 1, &monitorSensors_handle, app_cpu);
  xTaskCreatePinnedToCore (controlPG01, "PG02 Control", 2048, NULL, 1, &controlPG01_handle, app_cpu);
  xTaskCreatePinnedToCore (controlElectrodialysis, "Electrodialysis control", 2048, NULL, 1, &controlElectrodialysis_handle, app_cpu);
}

void loop() {
  // put your main code here, to run repeatedly:
}



void controlPG01(void* parameters){
  while (true){
    if (pG01Sensor.get_last_received_reading() > PRESSURE_LIMIT){
      Serial.println("PG01 is above 100psi");
    }
    vTaskDelay(6000/ portTICK_PERIOD_MS);
  }
}