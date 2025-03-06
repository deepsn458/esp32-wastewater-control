#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <time.h>
#include "electrodialysisControl.h"

LiquidCrystal_I2C LCD3 = LiquidCrystal_I2C(0x10, 16, 2);
LiquidCrystal_I2C LCD4 = LiquidCrystal_I2C(0x11, 16, 2);
LiquidCrystal_I2C LCD5 = LiquidCrystal_I2C(0x12, 16, 2);
LiquidCrystal_I2C LCD6 = LiquidCrystal_I2C(0x13, 16, 2);
LiquidCrystal_I2C LCD7 = LiquidCrystal_I2C(0x14, 16, 2);

TimerHandle_t controlPressTimer;
TimerHandle_t controlPressTimer2;

TaskHandle_t controlPH02Handle = NULL;
TaskHandle_t controlPress1Handle = NULL;
TaskHandle_t controlPress2Handle = NULL;
TaskHandle_t controlCellVoltageHandle = NULL;
TaskHandle_t controlLLS05Handle = NULL;
TaskHandle_t controlElectrochemicalVoltageHandle = NULL;

typedef struct pressureDevices{
    int pump;
    int pGSensor;
} pressureDevices;


//callback executes the  PG02 control and DC01,DC02 tasks after their respective delays
void startControlPressure(TimerHandle_t controlPressTimer){
    pressureDevices* pGDevices = (pressureDevices*) pvTimerGetTimerID(controlPressTimer);
    xTaskCreatePinnedToCore (controlPressure, "Pressure Control", 2048, pGDevices, 2,
        &controlPress1Handle, 1);
    xTaskCreatePinnedToCore (controlCellVoltage, "Cell Voltage Control", 2048, NULL, 4, 
        &controlCellVoltageHandle, 1);
}

//callback executes the PG03 Control, DC03, and Tank07 tasks after their respective delays
void startControlPressure2(TimerHandle_t controlPressTimer){
    pressureDevices* pGDevices = (pressureDevices*) pvTimerGetTimerID(controlPressTimer);
    xTaskCreatePinnedToCore (controlPressure, "Pressure Control", 2048, pGDevices, 2,
        &controlPress1Handle, 1);
    xTaskCreatePinnedToCore (controlElectrochemicalVoltage, 
        "Electrochemical Voltage Control", 2048, NULL, 3, 
        &controlElectrochemicalVoltageHandle, 1);
    xTaskCreatePinnedToCore (controlLLS05, "LLS05 Control", 2048, NULL, 2, 
        &controlLLS05Handle, 1);
    
}
void controlElectrodialysis(void* params){
    LCD3.init();
    LCD3.backlight();
    LCD4.init();
    LCD4.backlight();
    LCD5.init();
    LCD5.backlight();
    LCD6.init();
    LCD6.backlight();
    LCD7.init();
    LCD7.backlight();

    srand(time(NULL));

    //creates timer to start monitoring PG02
    static pressureDevices pg02Devices;
    pg02Devices.pGSensor = 2;
    pg02Devices.pump = 2;

    controlPressTimer = xTimerCreate(
        "Control PG02 Timer",
        5000/portTICK_PERIOD_MS,
        pdFALSE,
        (void* )&pg02Devices,
        startControlPressure
    );

    //creates timer to start monitoring PG03
    static pressureDevices pg03Devices;
    pg03Devices.pGSensor = 3;
    pg03Devices.pump = 5;

    controlPressTimer2 = xTimerCreate(
        "Control PG03 Timer",
        5000/portTICK_PERIOD_MS,
        pdFALSE,
        (void* )&pg03Devices,
        startControlPressure2
    );

    xTaskCreatePinnedToCore(controlPH02, "pH02 Control", 2048, NULL, 2, &controlPH02Handle, 1);
    
    for (;;){
        Serial.println("Tank 2 control Loop");
        vTaskDelay(60000/portTICK_RATE_MS);
    }

}

void controlPH02(void* params){
    const uint32_t dosingDelayMinutes = 1; // Dosing delay in minutes. Change as needed.
    const TickType_t dosingDelayTicks = dosingDelayMinutes * 10000 / portTICK_PERIOD_MS;
  
     TickType_t lastDoseTick = 0;
     xTimerStart(controlPressTimer, portMAX_DELAY);
     float pHValue = 8;
     LCD3.clear();
    LCD3.setCursor(0,0);
    LCD3.print("pH02 monitoring");
    while (true) {  
    // If pH is too high, check if it's time to dose again
      if (pHValue > 7) {
        if ((xTaskGetTickCount() - lastDoseTick) >= dosingDelayTicks) {
          lastDoseTick = xTaskGetTickCount();
          LCD3.clear();
          LCD3.setCursor(0,0);
          LCD3.print("pH too high");
          LCD3.setCursor(0,1);
          LCD3.print(pHValue);
          pHValue -= 0.5; 
      }
    }
      else if (pHValue < 6) {
        // pH is too low, send an alert
        
        Serial.print("Tank 2 pH reading is ");
        LCD3.clear();
        LCD3.setCursor(0,0);
        LCD3.print("pH too low");
        LCD3.setCursor(0,1);
        LCD3.print(pHValue);
      }
      else{
        Serial.print("Tank 2 pH reading is ");
        LCD3.clear();
        LCD3.setCursor(0,0);
        LCD3.print("pH is ok");
        LCD3.setCursor(0,1);
        LCD3.print(pHValue);
        Serial.println(pHValue);
      }
      
      vTaskDelay(10000 / portTICK_PERIOD_MS);
    
  }
  
}

void controlPressure(void* params){
    pressureDevices* pressDevices = (pressureDevices*) params;
    for(;;){

        if (pressDevices->pump == 5){
            LCD6.clear();
            LCD6.setCursor(0,0);
            LCD6.print("Pump");
            LCD6.setCursor(5,0);
            LCD6.print(pressDevices->pump);
            LCD6.setCursor(0,1);
            LCD6.print("PG");
            LCD6.setCursor(5,1);
            LCD6.print(pressDevices->pGSensor);
        }
        else{
            LCD3.clear();
            LCD3.setCursor(0,0);
            LCD3.print("Pump");
            LCD3.setCursor(5,0);
            LCD3.print(pressDevices->pump);
            LCD3.setCursor(0,1);
            LCD3.print("PG");
            LCD3.setCursor(5,1);
            LCD3.print(pressDevices->pGSensor);
        }
        
        vTaskDelay(4000/portTICK_PERIOD_MS);
    }
}

void controlCellVoltage(void* params){
    xTimerStart(controlPressTimer2, portMAX_DELAY);
    for (;;){
        LCD4.clear();
        LCD4.setCursor(0,0);
        LCD4.print("Cond 02: ");
        LCD4.setCursor(10,0);
        LCD4.print(rand());
        LCD4.setCursor(0,1);
        LCD4.print("DC01: ");
        LCD4.setCursor(10,1);
        LCD4.print(rand());

        LCD5.clear();
        LCD5.setCursor(0,0);
        LCD5.print("Cond 03: ");
        LCD5.setCursor(10,0);
        LCD5.print(rand());
        LCD5.setCursor(0,1);
        LCD5.print("DC02: ");
        LCD5.setCursor(10,1);
        LCD5.print(rand());
        vTaskDelay(6000/portTICK_PERIOD_MS);
    }
}

void controlElectrochemicalVoltage(void* params){
    for(;;){
        LCD6.clear();
        LCD6.setCursor(0,0);
        LCD6.print("DC03: ");
        vTaskDelay(7000/portTICK_PERIOD_MS);
    }

}
void controlLLS05(void* params){
    float lls5Level = 1.2;
  for(;;){
    if (lls5Level < 1.1){
      LCD7.clear();
      LCD7.setCursor(0,0);
      LCD7.print("LLS1 is low");
      LCD7.setCursor(0,1);
      LCD7.print(lls5Level);
      lls5Level +=2;
    }
    else if (lls5Level > 1.1){
      LCD7.clear();
      LCD7.setCursor(0,0);
      LCD7.print("LLS1 is high");
      LCD7.setCursor(0,1);
      LCD7.print(lls5Level);
      lls5Level -=2;
    }
    vTaskDelay(15000 / portTICK_PERIOD_MS);
  }
}

