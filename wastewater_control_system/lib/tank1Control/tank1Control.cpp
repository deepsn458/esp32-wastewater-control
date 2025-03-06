#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "tank1Control.h"

LiquidCrystal_I2C LCD1 = LiquidCrystal_I2C(0x27, 16, 2);
LiquidCrystal_I2C LCD8 = LiquidCrystal_I2C(0x15, 16, 2);

TaskHandle_t controlPH01Handle = NULL; 
TaskHandle_t controlLLS01Handle = NULL;

//for tank 8
TaskHandle_t controlLLS04Handle = NULL;


//==========================================================================================//
// Function to be implemented when the buzer task is called 
void controlTank01(void *params){
    LCD1.init();
    LCD1.backlight();
    LCD8.init();
    LCD8.backlight();
  xTaskCreatePinnedToCore(controlPH01, "pH01 Control", 2048, NULL, 1, &controlPH01Handle, 1);
  xTaskCreatePinnedToCore (controlLLS01, "LLS01 Control", 2048, NULL, 1, &controlLLS01Handle, 1);

  //for tank 8
  xTaskCreatePinnedToCore (controlLLS04, "LLS04 Control", 2048, NULL, 1, &controlLLS04Handle, 1);
  for(;;){
    Serial.println("Tank 1 control Loop");
    vTaskDelay(60000/portTICK_RATE_MS);
   
  }
}

//==============================================================================================//
// Function to be implemented the LED task is called 
void controlPH01(void* params){
  const uint32_t dosingDelayMinutes = 1; // Dosing delay in minutes. Change as needed.
  const TickType_t dosingDelayTicks = dosingDelayMinutes * 60000 / portTICK_PERIOD_MS;

   TickType_t lastDoseTick = 0;
   float pHValue = 8;
  while (true) {  
    

  // If pH is too high, check if it's time to dose again
    if (pHValue > 7) {
      Serial.print("Tank 1 pH reading is");
      Serial.println(pHValue);
      if ((xTaskGetTickCount() - lastDoseTick) >= dosingDelayTicks) {
           
        lastDoseTick = xTaskGetTickCount();
        LCD1.clear();
        LCD1.setCursor(0,0);
        LCD1.print("pH too high");
        LCD1.setCursor(0,1);
        LCD1.print(pHValue);
        pHValue -= 0.5;
    }
  }
    else if (pHValue < 6) {
      // pH is too low, send an alert
      Serial.print("Tank 1 pH reading is ");
      LCD1.clear();
      LCD1.setCursor(0,0);
      LCD1.print("pH too low");
      LCD1.setCursor(0,1);
      LCD1.print(pHValue);
      Serial.println(pHValue);
    }
    else{
      Serial.print("Tank 1 pH reading is ");
      LCD1.clear();
      LCD1.setCursor(0,0);
      LCD1.print("pH is ok");
      LCD1.setCursor(0,1);
      LCD1.print(pHValue);
      Serial.println(pHValue);
    }
    
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  
}
  
}

void controlLLS01(void* params){
  float lls1Level = 1.2;
  for(;;){
    if (lls1Level < 1.1){
      LCD1.clear();
      LCD1.setCursor(0,0);
      LCD1.print("LLS1 is low");
      LCD1.setCursor(0,1);
      LCD1.print(lls1Level);
      lls1Level +=2;
    }
    else if (lls1Level > 1.1){
      LCD1.clear();
      LCD1.setCursor(0,0);
      LCD1.print("LLS1 is high");
      LCD1.setCursor(0,1);
      LCD1.print(lls1Level);
      lls1Level -=2;
    }
    vTaskDelay(15000 / portTICK_PERIOD_MS);
  }
}

void controlLLS04(void* params){
  float lls4Level = 1.2;
  for(;;){
    if (lls4Level < 1.1){
      LCD8.clear();
      LCD8.setCursor(0,0);
      LCD8.print("LLS1 is low");
      LCD8.setCursor(0,1);
      LCD8.print(lls4Level);
      lls4Level +=2;
    }
    else if (lls4Level > 1.1){
      LCD8.clear();
      LCD8.setCursor(0,0);
      LCD8.print("LLS1 is high");
      LCD8.setCursor(0,1);
      LCD8.print(lls4Level);
      lls4Level -=2;
    }
    vTaskDelay(15000 / portTICK_PERIOD_MS);
  }
}