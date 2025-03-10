#include <Arduino.h>
#include <esp_timer.h>
#include <DFRobot_RTU.h>


const int PUMP_ON = 1;
const int PUMP_OFF =0;

//UART Serial Pins
#define RXD2 16
#define TXD2 17
/*TODO set each kamoer pump a unique Modbus Address using the rotary encoder*/
#define PUMP01_ADDR ((uint16_t)0xC0)
#define PUMP02_ADDR ((uint16_t)0xC1)
#define PUMP03_ADDR ((uint16_t)0xC2)
#define PUMP04_ADDR ((uint16_t)0xC3)
#define PUMP05_ADDR ((uint16_t)0xC4)

//sets up the serial port for RS485 Communication
HardwareSerial rs485Serial(2);

//creates modbus object for rs485 communication
DFRobot_RTU modbus(&rs485Serial);


void controlPumps(void* parameters){
    rs485Serial.begin(9600, SERIAL_8N1, RXD2, TXD2);

    //sets up the MODBUS RTU communication for the pumps
    modbus.writeCoilsRegister(PUMP01_ADDR, 0x1004, 1);
    modbus.writeCoilsRegister(PUMP02_ADDR, 0x1004, 1);
    modbus.writeCoilsRegister(PUMP03_ADDR, 0x1004, 1);
    modbus.writeCoilsRegister(PUMP04_ADDR, 0x1004, 1);
    modbus.writeCoilsRegister(PUMP05_ADDR, 0x1004, 1);

    for(;;){
        //turns pump on or off
    modbus.writeCoilsRegister(PUMP01_ADDR, 0x1001,PUMP_ON);
    delay(50);

    //sets the speed of the pump to 100.0rpm for now
    modbus.writeHoldingRegister(PUMP01_ADDR,0x3001,0x42C8);
    delay(50);
    modbus.writeHoldingRegister(PUMP01_ADDR,0x3002, 0x0000);
    delay(50);
    vTaskDelay(1000/ portTICK_PERIOD_MS);
    }
}