/*In this basic free RTOS tutorial will use xTaskCreatePinnedToCore() . 
in order to enable us to select which ESP32 core (core 0 or core 1) will run the particular task.*/

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C LCD = LiquidCrystal_I2C(0x27, 16, 2);

TaskHandle_t controlTank01Handle = NULL;    // Task handler for the buzer function
TaskHandle_t controlPH01Handle = NULL; 
TaskHandle_t controlLLS01Handle = NULL;


void controlPH01(void* params);
void controlLLS01(void* params);
//==========================================================================================//
// Function to be implemented when the buzer task is called 
void controlTank01(void *params){
  xTaskCreatePinnedToCore(controlPH01, "pH01 Control", 2048, NULL, 1, &controlPH01Handle, 1);
  xTaskCreatePinnedToCore (controlLLS01, "LLS01 Control", 2048, NULL, 1, &controlLLS01Handle, 1);
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

  while (true) {  
    float pHValue = 5;

  // If pH is too high, check if it's time to dose again
    if (pHValue > 7) {
      Serial.print("Tank 1 pH reading is");
      Serial.println(pHValue);
      if ((xTaskGetTickCount() - lastDoseTick) >= dosingDelayTicks) {
      
        pHValue += 0.1;       
        lastDoseTick = xTaskGetTickCount();
        LCD.clear();
        LCD.setCursor(0,0);
        LCD.print("pH too high");
        LCD.setCursor(0,1);
        LCD.print(pHValue);
    }
  }
    else if (pHValue < 6) {
      // pH is too low, send an alert
      Serial.print("Tank 1 pH reading is ");
      LCD.clear();
      LCD.setCursor(0,0);
      LCD.print("pH too low");
      LCD.setCursor(0,1);
      LCD.print(pHValue);
      Serial.println(pHValue);
    }
    else{
      Serial.print("Tank 1 pH reading is ");
      LCD.clear();
      LCD.setCursor(0,0);
      LCD.print("pH is ok");
      LCD.setCursor(0,1);
      LCD.print(pHValue);
      Serial.println(pHValue);
    }
    
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  
}
  
}

void controlLLS01(void* params){
  float lls1Level = 1.2;
  for(;;){
    if (lls1Level < 1.1){
      LCD.clear();
      LCD.setCursor(0,0);
      LCD.print("LLS1 is low");
      LCD.setCursor(0,1);
      LCD.print(lls1Level);
      lls1Level +=2;
    }
    else if (lls1Level > 1.1){
      LCD.clear();
      LCD.setCursor(0,0);
      LCD.print("LLS1 is high");
      LCD.setCursor(0,1);
      LCD.print(lls1Level);
      lls1Level -=2;
    }
    vTaskDelay(15000 / portTICK_PERIOD_MS);
  }
}

//===========================================================================================//
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  LCD.init();
  LCD.backlight();
 
  Serial.println("Hello, ESP32 with FreeRTOS !");
  xTaskCreatePinnedToCore (controlTank01, "Tank01 Control", 2048, NULL, 1, &controlTank01Handle, 1);
}

void loop() {
 

}
