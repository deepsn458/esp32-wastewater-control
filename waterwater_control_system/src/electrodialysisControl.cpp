#include <Arduino.h>
#include <esp_timer.h>
#include "electrodialysisControl.h"

/*PUMP DIRECTIONS*/
const int PUMP_ON = 1;
const int PUMP_OFF =0;
const int PRESSURE_LIMIT = 100;
const int SHUTDOWN_VOLTAGE = 75;
/*TODO: Import the different pins corresponding to the different sensors/actuators*/
int pump1Pin = 1;
int pump2Pin = 2;
int pump3Pin =  3;
int pump4Pin = 4;
int pump5Pin = 5;
int pH02Pin = 6;
int acidPump1Pin = 7;
int acidPump2Pin = 8;
int pG02Pin = 9;
int pG03Pin = 17;
int dC01Pin = 10;
int dC02Pin = 11;
int dC03Pin = 12;
int cond02Pin = 13;
int cond03Pin = 14;
int lowLevelPin = 15;
int highLevelPin = 16;

struct pressureDevices{
    int pumpPin;
    int pGPin;
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
        pG02Pin
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
        pG03Pin
    };
    xTaskCreatePinnedToCore (controlPressure, "Pressure Control", 2048, &pG05Devices, 1,
         &controlPress2Handle, app_cpu);
        xTaskCreatePinnedToCore (controlLLS05, "LLS05 Control", 2048, NULL, 1, 
            &controlLLS05Handle, app_cpu);
}





void controlElectrodialysis(void* parameters){
    //spawns the first task
    xTaskCreatePinnedToCore (controlPH02, "pH02 Monitoring", 2048, NULL, 1, NULL, app_cpu);
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
    
}

void controlPH02(void* parameters){
    /*TODO: create timer based on user input to delay when controlPressure Starts*/
    xTimerStart(controlPress1Timer, portMAX_DELAY);
    for(;;){
        pumpControl(pump1Pin, PUMP_ON);

        //waits for one minute
        vTaskDelay(60000/ portTICK_PERIOD_MS);

        if (readPH(pH02Pin) > 5.4){
            acidPumpControl(acidPump1Pin, PUMP_ON);
            vTaskDelay(60000/ portTICK_PERIOD_MS);
        }
        else{
            acidPumpControl(acidPump1Pin, PUMP_OFF);
            if (readPH(pH02Pin) < 5.0){
                sendAlert("pH in tank 2 is below 5");
            }
        }
    }
    
}

void controlPressure(void* parameters){
    struct pressureDevices* pressDevices = (struct pressureDevices*) parameters;
    for(;;){
        /*TODO make the pump and the pg pins parameters*/
        pumpControl(pressDevices->pumpPin, PUMP_ON);
        if (readPressure(pressDevices->pGPin) < (float)PRESSURE_LIMIT){
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
        if (readVoltage(dC01Pin) > SHUTDOWN_VOLTAGE){
            sendAlert("DC01 Voltage is above the threshold");
            /*TODO Implement shutdown interrupt*/
        }
        if (readVoltage(dC02Pin)> SHUTDOWN_VOLTAGE){
            sendAlert("DC02 Voltage is above the threshold");
            /*TODO Implement shutdown interrupt*/
        }

        if (readConductivity(cond02Pin)< 23){
            sendAlert("C02 is below the threshold");
        }
        if (readConductivity(cond03Pin)< 23){
            sendAlert("C03 is below the threshold");
        }
        vTaskDelay(60000/portTICK_PERIOD_MS);
        
    }

}


void controlLLS05(void* parameters){
    for(;;){
        if (!readLiquidLevel(lowLevelPin)){
            sendAlert("liquid is below low level");
        }
        else if (readLiquidLevel(highLevelPin)){
            sendAlert("liquid is above high level");
        }
        vTaskDelay(60000/portTICK_PERIOD_MS);
    }
}

void IRAM_ATTR systemShutdown(){
    Serial.println("Overvoltage condition! System shutdown");
}





