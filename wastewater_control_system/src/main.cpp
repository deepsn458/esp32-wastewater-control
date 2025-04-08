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
Ezo_board pHSensor = Ezo_board(99, "pH02");
#define DOSING_PUMP_ADDRESS_1 0x38  // Example address for Tank 1 dosing pump
Ezo_board dosingPumpTank01 = Ezo_board(DOSING_PUMP_ADDRESS_1, "PMP1");
float phReading;
float last_reading;
int same_reading;
char calibration_status[32];
void setup() {
  Serial.begin(115200);
  sensor.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Wire.begin();
  TaskHandle_t sensorMonitoringTask_handle= NULL;
  TaskHandle_t dataLogging_handle = NULL;
  TaskHandle_t controlPumps_handle = NULL;
  TaskHandle_t doseAcid_handle = NULL;

  //pHSensor.send_cmd("Cal,clear");
    
  //xTaskCreatePinnedToCore(sensorMonitoringTask,"Sensor Monitoring",2048, NULL, 1, &sensorMonitoringTask_handle, 1);
  //xTaskCreatePinnedToCore(controlPumps, "Data Logging", 4096, NULL, 1, &controlPumps_handle, 1);
  //xTaskCreatePinnedToCore(doseAcid, "Dose Acid", 2048, NULL, 2,&doseAcid_handle,1);

}

void loop() {
  dosingPumpTank01.send_cmd_with_num("d,", 10);
  /*
  sensor.print("I2C,109");
  sensor.print("\r");
  if (sensor.available()>0){
    Serial.println(sensor.read());
  }
    */
  /*
  pHSensor.send_read_cmd();
  delay(1000);
  pHSensor.receive_read_cmd();
  Serial.println(pHSensor.get_last_received_reading());
  if (last_reading - pHSensor.get_last_received_reading() < 0.2 ){
    same_reading++;
    if (same_reading == 5){
      pHSensor.send_cmd("Cal,high,10");
      delay(1100);
      pHSensor.send_cmd("Cal,?");
      delay(500);
      pHSensor.receive_cmd(calibration_status,32);
      Serial.println(calibration_status);
    }
  }
  else{
    same_reading = 0;
  }
  last_reading = pHSensor.get_last_received_reading();
  */
 // Change dosing volume as needed, units is in mL
 delay(2000);
}