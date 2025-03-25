#include <Arduino.h>
#include <Ezo_i2c.h>
#include "tankControl.h"
#include "sensorMonitoring.h"
#include "electrodialysisControl.h"
#include <Wire.h>

#define PRESSURE_LIMIT 100
/*change to 114*/
const int PG01_SENSOR_ADDRESS = 114;
const int LLS04LowPin = 14;
const int LLS04HighPin = 35;

Ezo_board pG01Sensor = Ezo_board(PG01_SENSOR_ADDRESS, "pG01");
/* Using core 1 of ESP32 */
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

//void setupHardware();
//void setupNetwork();

TaskHandle_t monitorSensors_handle = NULL;
TaskHandle_t controlElectrodialysis_handle = NULL;
//UART Serial Pins
#define RXD2 16
#define TXD2 17
HardwareSerial sensor(2);

//run the Urinal& Strainer PG monitoring and the Tank 8 control in the main task
void monitorPG01();
void monitorLLS04();

void setup() {
  // set up all the sensors and drivers and the network
  //setupHardware();
  //setupNetwork();
  //wait for start signal
  /*TODO: Implement Start signal*/

  Serial.begin(115200);
  sensor.begin(9600, SERIAL_8N1, RXD2, TXD2);
  pinMode(LLS04HighPin, INPUT_PULLUP);
  pinMode(LLS04LowPin, INPUT_PULLUP);
  Wire.begin();
  //controlTank1();
  //xTaskCreatePinnedToCore (monitorSensors, "Sensor Monitoring", 2048, NULL, 1, &monitorSensors_handle, app_cpu);
 //controlElectrodialysis();
  
  
  
}

void loop() {
  
  sensor.print("I2C,106");
  sensor.print("\r");
  if (sensor.available()>0){
    Serial.println(sensor.read());
    Serial.println("ye");
  }
  
 /*
  monitorPG01();
  
  vTaskDelay(500/portTICK_PERIOD_MS);
  monitorLLS04();
  vTaskDelay(6000/portTICK_PERIOD_MS);
  */
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
