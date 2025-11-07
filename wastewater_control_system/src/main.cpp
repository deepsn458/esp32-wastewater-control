#include <Arduino.h>
#include <WiFi.h>
#include "database.h"
#include <FirebaseClient.h>
#include <Ezo_i2c.h>
#include "tankControl.h"
#include "sensorMonitoring.h"
#include "electrodialysisControl.h"
#include <Wire.h>
#include <algorithm>

//UART Serial Pins
#define RXD2 16
#define TXD2 17
#define DC01 1
#define DC02 2
#define DCO3 3

//for serial port expander
const int serialSelect1 = 32;
const int serialSelect2 = 33;
const int serialSelect3 = 25;

HardwareSerial PSUSerial(2);
#define PRESSURE_LIMIT 100


TaskHandle_t monitorSensorsHandle = NULL;
TaskHandle_t controlPH01Handle = NULL;
TaskHandle_t controlPH02Handle = NULL;
TaskHandle_t monitorVoltageHandle = NULL;
Ezo_board pHSensor_adjustment = Ezo_board(102, "PH_ADJ");
#define DOSING_PUMP_ADDRESS_1 0x39  // Example address for Tank 1 dosing pump
Ezo_board dosingPumpTank01 = Ezo_board(DOSING_PUMP_ADDRESS_1, "PMP1");
/* Using core 1 of ESP32 */
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

void phControl(void* params);
void monitorVoltage(void* params);
void checkVoltage(int psuID);
#define WIFI_SSID "Rice Visitor"

const char* DATABASE_URL = "https://waste-water-control-default-rtdb.firebaseio.com/";

void setup(){
  Wire.begin();
  Serial.begin(115200);
  PSUSerial.begin(9600, SERIAL_8N1, RXD2, TXD2);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print(WiFi.localIP());
  initFirebase(DATABASE_URL);
  xTaskCreatePinnedToCore(phControl,"PH Control", 10000,NULL,3, &controlPH02Handle, app_cpu);
  xTaskCreatePinnedToCore(monitorSensors,"Monitor Sensors", 10000, NULL, 1,&monitorSensorsHandle, app_cpu);
  xTaskCreatePinnedToCore(monitorVoltage, "Monitor Voltage", 10000, NULL, 2, &monitorVoltageHandle, app_cpu);
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
    vTaskDelay(3000/portTICK_PERIOD_MS);
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



void monitorVoltage(void* params){
    for(;;){
        checkVoltage(DC01);
        vTaskDelay(500/portTICK_PERIOD_MS);
        checkVoltage(DC02);
        vTaskDelay(500/portTICK_PERIOD_MS);
        checkVoltage(DCO3);
        vTaskDelay(10000/portTICK_PERIOD_MS);  
    }

}

void checkVoltage(int psuID){
    //selects the appropriate serial port
    char psuName[50];
    sprintf(psuName,"DC_0%d",psuID);
    if (psuID == 1){
        digitalWrite(serialSelect1, 0);
        digitalWrite(serialSelect2,0);
        digitalWrite(serialSelect3, 0);
    }
    else if (psuID == 2){
        digitalWrite(serialSelect1, 0);
        digitalWrite(serialSelect2,0);
        digitalWrite(serialSelect3, 1);
    }
    if (psuID == 3){
        digitalWrite(serialSelect1, 0);
        digitalWrite(serialSelect2,1);
        digitalWrite(serialSelect3, 0);
    }
    PSUSerial.print("VOLTage?\r\n");
    int i = 0;
    char voltReading[10];
    while (PSUSerial.available()>0){
      voltReading[i] = PSUSerial.read();
      i++;
    }
    i = 0;
    float voltage = atof(voltReading);
    Serial.println(voltage);
    pushSensorReading("voltage",psuName,voltage);
    /*
    PSUSerial.print("CURRent?\r\n");
    char currReading[10];
    while (PSUSerial.available()>0){
        currReading[i] = PSUSerial.read();
        i++;
    }
    i = 0;
    float current = atof(currReading);
    pushSensorReading("current",psuName,current);

    //either the system is on the verge of shutdown, or it has already been shutdown by
    //the PSU's OVP system
    if (voltage >= SHUTDOWN_VOLTAGE || voltage <= 0.0){
        char alert[50];
        sprintf(alert, "DC 0%d Overvoltage condition",psuID);
        pushAlert(alert);
        Serial.println(alert);
    }
    Serial.println(voltReading);
    delay(1000);
    */

    
}

