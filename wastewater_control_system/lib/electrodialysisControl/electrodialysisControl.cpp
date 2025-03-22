#include "Arduino.h"
#include <esp_timer.h>
#include <Ezo_i2c.h>
#include <Wire.h>
#include <DFRobot_RTU.h>
#include "electrodialysisControl.h"

/* Using core 1 of ESP32 */
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

/*PUMP DIRECTIONS*/
const int PUMP_ON = 1;
const int PUMP_OFF =0;
const int PRESSURE_LIMIT = 100;
const int SHUTDOWN_VOLTAGE = 75;

//UART Serial Pins
#define RXD2 16
#define TXD2 17
/*TODO set each kamoer pump a unique Modbus Address using the rotary encoder*/
#define PUMP01_ADDR ((uint16_t)0xC0)
#define PUMP02_ADDR ((uint16_t)0xC1)
#define PUMP03_ADDR ((uint16_t)0xC2)
#define PUMP04_ADDR ((uint16_t)0xC3)
#define PUMP05_ADDR ((uint16_t)0xC4)

const int dC01Pin = 12;
const int dC02Pin = 13;
const int dC03Pin = 14;
const int lowLevelPin = 15;
const int highLevelPin = 16;
const int PG02_SENSOR_ADDRESS = 110;
const int PG03_SENSOR_ADDRESS = 111;
const int Cond02_SENSOR_ADDRESS = 112;
const int Cond03_SENSOR_ADDRESS = 113;

//sets up the serial port for RS485 Communication
HardwareSerial rs485Serial(2);

//creates modbus object for rs485 communication
DFRobot_RTU modbus(&rs485Serial);
/*TODO: Assign each sensors a unique I2C Address*/
Ezo_board pG02Sensor = Ezo_board(PG02_SENSOR_ADDRESS, "pG02");
Ezo_board pG03Sensor = Ezo_board(PG03_SENSOR_ADDRESS, "pG03");

Ezo_board cond02Sensor = Ezo_board(Cond02_SENSOR_ADDRESS, "cond02");
Ezo_board cond03Sensor = Ezo_board(Cond03_SENSOR_ADDRESS, "cond03");
typedef struct pressureDevices{
    uint16_t pumpAddr;
    Ezo_board pGSensor;
} pressureDevices;

TimerHandle_t controlPress1Timer;
TimerHandle_t controlPress2Timer;


TaskHandle_t controlPress1Handle = NULL;
TaskHandle_t controlPress2Handle = NULL;
TaskHandle_t controlCellVoltageHandle = NULL;
TaskHandle_t controlLLS05Handle = NULL;



//callback function when the timer expires
void startControlPress1(TimerHandle_t controlPress1Timer){
    pressureDevices pG02Devices = {
        PUMP02_ADDR,
        pG02Sensor
    };
    xTaskCreatePinnedToCore (controlPressure, "Pressure Control", 1024, &pG02Devices, 1,
         &controlPress1Handle, app_cpu);
    xTaskCreatePinnedToCore (controlCellVoltage, "Cell Voltage Control", 1024, NULL, 1, 
        &controlCellVoltageHandle, app_cpu);
}

//callback function when the timer expires
void startControlPress2(TimerHandle_t controlPress2Timer){
    pressureDevices pG05Devices = {
        PUMP05_ADDR,
        pG03Sensor
    };
    xTaskCreatePinnedToCore (controlPressure, "Pressure Control", 1024, &pG05Devices, 1,
         &controlPress2Handle, app_cpu);
        xTaskCreatePinnedToCore (controlLLS05, "LLS05 Control", 1024, NULL, 1, 
            &controlLLS05Handle, app_cpu);
}

void controlElectrodialysis(){
    
    rs485Serial.begin(9600, SERIAL_8N1, RXD2, TXD2);
    //sets up the RS485 Communication with the pumps
    setupPumps();

    //Hardware interrupts for overvoltage conditions
    attachInterrupt(dC01Pin, systemShutdown, HIGH);
    attachInterrupt(dC02Pin, systemShutdown, HIGH);
    attachInterrupt(dC03Pin, systemShutdown, HIGH);

    //creates the timers
    controlPress1Timer = xTimerCreate(
        "Control Pressure1 Timer",
        //change to 1 minute
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

    
    //turn on pump 1
    pumpControl(PUMP01_ADDR, PUMP_ON);
    
}

void controlPressure(void* parameters){
    pressureDevices* pressDevices = (pressureDevices*) parameters;
    for(;;){
        /*TODO make the pump and the pg pins parameters*/
        pumpControl(pressDevices->pumpAddr, PUMP_ON);
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
        pumpControl(PUMP03_ADDR, PUMP_ON);
        pumpControl(PUMP04_ADDR, PUMP_ON);
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
void setupPumps(){
    modbus.writeCoilsRegister(PUMP01_ADDR, 0x1004, 1);
    modbus.writeCoilsRegister(PUMP02_ADDR, 0x1004, 1);
    modbus.writeCoilsRegister(PUMP03_ADDR, 0x1004, 1);
    modbus.writeCoilsRegister(PUMP04_ADDR, 0x1004, 1);
    modbus.writeCoilsRegister(PUMP05_ADDR, 0x1004, 1);
}

void pumpControl(uint16_t pumpAddr, int direction){

    //turns pump on or off
    modbus.writeCoilsRegister(pumpAddr, 0x1001,PUMP_ON);
    delay(50);

    //sets the speed of the pump to 400.0rpm for now
    modbus.writeHoldingRegister(pumpAddr,0x3001,0x43C8);
    delay(50);
    modbus.writeHoldingRegister(pumpAddr,0x3002, 0x0000);
    delay(50);
}
