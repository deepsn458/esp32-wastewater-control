#define RXD2 16
#define TXD2 17
#include <stdlib.h>
#include <Arduino.h>
HardwareSerial PSUSerial(2);

void getVoltage(){
    Serial.begin(115200);
    PSUSerial.begin(9600,SERIAL_8N1,RXD2,TXD2);

    while (true){
        PSUSerial.print("FORMat ASCII\r\n");
        //PSUSerial.print("*IDN?\r\n");
        PSUSerial.print("VOLTage 3\r\n");
        PSUSerial.print("OUTPut ON\r\n");
        PSUSerial.print("VOLTage:PROTection:STATe OFF\r\n");
        PSUSerial.print("VOLTage?\r\n");
        int i = 0;
        char voltReading[10];
        
        while (PSUSerial.available()>0){
            voltReading[i] = PSUSerial.read();
            i++;
        }
        i = 0;
        float voltage = atof(voltReading);
        if (voltage > 2.9){
            Serial.println("ovp");
        }
        delay(1000);
    }
}
