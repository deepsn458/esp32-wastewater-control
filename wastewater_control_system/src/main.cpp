#include <Arduino.h>
#include "tankControl.h"
#include "sensorMonitoring.h"
#include "electrodialysisControl.h"

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

  
  xTaskCreatePinnedToCore (controlTank01, "Tank01 Monitoring", 2048, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore (monitorSensors, "Sensor Monitoring", 2048, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore (controlPG01, "PG02 Control", 2048, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore (controlElectrodialysis, "Electrodialysis control", 2048, NULL, 1, NULL, app_cpu);
}

void loop() {
  // put your main code here, to run repeatedly:
}



void controlPG01(void* parameters){
  while (true){
    Serial.println("Monitoring PG01");
    vTaskDelay(1000/ portTICK_PERIOD_MS);
  }
}