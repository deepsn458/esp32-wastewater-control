#include "Arduino.h"
#include <esp_timer.h>
#include <Ezo_i2c.h>
#include <Wire.h>
#include "electrodialysisControl.h"

/*PUMP DIRECTIONS*/
const int PUMP_ON = 1;
const int PUMP_OFF =0;
const int PRESSURE_LIMIT = 100;
const int SHUTDOWN_VOLTAGE = 75;
/*TODO: Import the different pins corresponding to the different sensors/actuators*/
Ezo_board pG02Sensor = Ezo_board(PG02_SENSOR_ADDRESS, "pG02");
Ezo_board pG03Sensor = Ezo_board(PG03_SENSOR_ADDRESS, "pG03");

Ezo_board cond02Sensor = Ezo_board(Cond02_SENSOR_ADDRESS, "cond02");
Ezo_board cond03Sensor = Ezo_board(Cond03_SENSOR_ADDRESS, "cond03");

const int pump1Pin = 1;
const int pump2Pin = 2;
const int pump3Pin =  3;
const int pump4Pin = 4;
const int pump5Pin = 5;
const int pH02Pin = 6;
const int acidPump1Pin = 7;
const int acidPump2Pin = 8;
const int pG02Pin = 9;
const int pG03Pin = 17;
const int dC01Pin = 10;
const int dC02Pin = 11;
const int dC03Pin = 12;
const int cond02Pin = 13;
const int cond03Pin = 14;
const int lowLevelPin = 15;
const int highLevelPin = 16;

struct pressureDevices{
    int pumpPin;
    Ezo_board pGSensor;
};

TimerHandle_t controlPress1Timer;
TimerHandle_t controlPress2Timer;

TaskHandle_t controlPress1Handle = NULL;
TaskHandle_t controlPress2Handle = NULL;
TaskHandle_t controlCellVoltageHandle = NULL;
TaskHandle_t controlLLS05Handle = NULL;

/* Using core 1 of ESP32 */
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

//callback function when the timer expires
void startControlPress1(TimerHandle_t controlPress1Timer){
    struct pressureDevices pG02Devices = {
        pump1Pin,
        pG02Sensor
    };
    xTaskCreatePinnedToCore (controlPressure, "Pressure Control", 2048, &pG02Devices, 1,
         &controlPress1Handle, app_cpu);
    xTaskCreatePinnedToCore (controlCellVoltage, "Cell Voltage Control", 2048, NULL, 1, 
        &controlCellVoltageHandle, app_cpu);
}

//callback function when the timer expires
void startControlPress2(TimerHandle_t controlPress2Timer){
    struct pressureDevices pG05Devices = {
        pump5Pin,
        pG03Sensor
    };
    xTaskCreatePinnedToCore (controlPressure, "Pressure Control", 2048, &pG05Devices, 1,
         &controlPress2Handle, app_cpu);
        xTaskCreatePinnedToCore (controlLLS05, "LLS05 Control", 2048, NULL, 1, 
            &controlLLS05Handle, app_cpu);
}
void controlElectrodialysis(void* parameters){

    //sets up the sensors
    pG02Sensor.send_read_cmd();
    pG03Sensor.send_read_cmd();
    cond02Sensor.send_read_cmd();
    cond03Sensor.send_read_cmd();

    //Hardware interrupts for overvoltage conditions
    attachInterrupt(dC01Pin, systemShutdown, HIGH);
    attachInterrupt(dC02Pin, systemShutdown, HIGH);
    attachInterrupt(dC03Pin, systemShutdown, HIGH);

    //creates the timers
    controlPress1Timer = xTimerCreate(
        "Control Pressure1 Timer",
        1000/portTICK_PERIOD_MS,
        pdFALSE,
        (void*)0,
        startControlPress1
    );
    controlPress2Timer = xTimerCreate(
        "Control Pressure2 Timer",
        1000/portTICK_PERIOD_MS,
        pdFALSE,
        (void*)0,
        startControlPress2
    );
    //create timer based on user input to delay when controlPressure Starts
    xTimerStart(controlPress1Timer, portMAX_DELAY);
    
}

void controlPressure(void* parameters){
    struct pressureDevices* pressDevices = (struct pressureDevices*) parameters;
    for(;;){
        /*TODO make the pump and the pg pins parameters*/
        pumpControl(pressDevices->pumpPin, PUMP_ON);
        if (readSensor(pressDevices->pGSensor) < (float)PRESSURE_LIMIT){
            sendAlert("PG02 pressure is below 100");
        }
        vTaskDelay(60000/portTICK_PERIOD_MS);
    }
}

void controlCellVoltage(void* parameters){
    /*TODO: create timer based on user input to delay when the second pressure control loop Starts*/
    xTimerStart(controlPress2Timer, portMAX_DELAY);
    for(;;){
        pumpControl(pump3Pin, PUMP_ON);
        pumpControl(pump4Pin, PUMP_ON);
        Serial.println("Turning on UV Lamp");
    
        if (readSensor(cond02Sensor)< 23){
            sendAlert("C02 is below the threshold");
        }
        if (readSensor(cond03Sensor)< 23){
            sendAlert("C02 is below the threshold");
        }
        vTaskDelay(60000/portTICK_PERIOD_MS);
        
    }

}
void controlLLS05(void* parameters){
    for(;;){
        if (readLiquidLevel(lowLevelPin) == 0){
            sendAlert("liquid is below low level");
        }
        else if (readLiquidLevel(highLevelPin) == 1){
            sendAlert("liquid is above high level");
        }
        vTaskDelay(60000/portTICK_PERIOD_MS);
    }
}
void IRAM_ATTR systemShutdown(){
    Serial.println("Overvoltage condition! System shutdown");
}

float readSensor(Ezo_board sensor){
    return sensor.get_last_received_reading();
}

int readLiquidLevel(int gpioPin){
    return digitalRead(gpioPin);
}

/*TODO: Control peristaltic pumps with Modbus-rtu */
