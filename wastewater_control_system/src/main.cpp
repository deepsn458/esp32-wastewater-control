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
//sets up the serial port for RS485 Communication

#define PRESSURE_LIMIT 100
float pHVal = 0;
float condVal = 0;
int same_read_count = 0;
int cal_count = 0;

const int PG01_SENSOR_ADDRESS = 114;
const int PH_SENSOR_ADDRESS_ADJUSTMENT = 102;
const int COND_ED2_SENSOR_ADDRESS = 0x69;
const int COND_ED1_SENSOR_ADDRESS = 0x70;
const int LLS04LowPin = 14;
const int LLS04HighPin = 35;
TaskHandle_t monitorSensorsHandle = NULL;
TaskHandle_t controlPH01Handle = NULL;
TaskHandle_t controlPH02Handle = NULL;
Ezo_board pG01Sensor = Ezo_board(PG01_SENSOR_ADDRESS, "pG01");
Ezo_board pHSensor_adjustment = Ezo_board(PH_SENSOR_ADDRESS_ADJUSTMENT, "PH_ADJ");
Ezo_board condEd2_sensor = Ezo_board(COND_ED2_SENSOR_ADDRESS, "COND_ED2");
Ezo_board condEd1_sensor = Ezo_board(COND_ED1_SENSOR_ADDRESS, "COND_ED1");

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

  pinMode(LLS04HighPin, INPUT_PULLUP);
  pinMode(LLS04LowPin, INPUT_PULLUP);
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


}



void loop() {
  
  condEd2_sensor.send_read_cmd();
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  condEd2_sensor.receive_read_cmd();
  Serial.println(condEd2_sensor.get_last_received_reading());
  /*
  if (cal_count == 0){
    condEd2_sensor.send_cmd("Cal,clear");
  }
  condEd2_sensor.send_read_cmd();
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  // Assume get_reading() returns a String with the sensor's response.
  condEd2_sensor.receive_read_cmd();
  Serial.println(condEd2_sensor.get_last_received_reading());
  if ((condVal - (float)condEd2_sensor.get_last_received_reading())<1.0){
    same_read_count++;
  }
  else{
    same_read_count = 0;
  }
  condVal = condEd2_sensor.get_last_received_reading();
  Serial.println(condVal);
  if (same_read_count == 5){
    if (cal_count == 0){
      Serial.println("dry cal");
       condEd2_sensor.send_cmd("Cal,dry");
       same_read_count = 0;
    }
    else if (cal_count == 1){
      Serial.println("cal 84");
      condEd2_sensor.send_cmd("Cal,84");
      same_read_count = 0;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    condEd2_sensor.send_cmd("Cal,?");
    
    vTaskDelay(300 / portTICK_PERIOD_MS);
    char cal[15];
    condEd2_sensor.receive_cmd(cal,15);
    Serial.println(cal);
    vTaskDelay(60000 / portTICK_PERIOD_MS);
    cal_count++;
  
  }
  */
  // Keep Firebase tasks running
  fireBaseLoop();
  //pushSensorReading("pressure", "PG01",89);
  //pushSensorReading("PH",pHSensor.get_name(), pHSensor.get_last_received_reading());
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
