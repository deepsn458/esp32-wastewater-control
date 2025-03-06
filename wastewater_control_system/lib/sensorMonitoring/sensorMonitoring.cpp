#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "sensorMonitoring.h"
LiquidCrystal_I2C LCD2 = LiquidCrystal_I2C(0x28, 16, 2);

void monitorSensors(void* params){
    LCD2.init();
    LCD2.backlight();
    for (;;){
        LCD2.clear();
        LCD2.setCursor(0,0);
        LCD2.print("monitoring");
        LCD2.setCursor(0,1);
        LCD2.print("sensors");

        vTaskDelay(8000/portTICK_PERIOD_MS);
    }
}