#include <Arduino.h>
#include "tankControl.h"
#include "Ezo_i2c.h"   // Atlas EZO I2C library for controlling the pump
#include "Wire.h"
/* Using core 1 of ESP32 */
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Task handles for Tank 1
TaskHandle_t pH01Control_handle = NULL;
TaskHandle_t LLS01Control_handle = NULL;

// Task handles for Tank 2
TaskHandle_t pH02Control_handle = NULL;
TaskHandle_t LLS02Control_handle = NULL;

// Create an instance of the dosing pumps
Ezo_board dosingPump_Tank01 = Ezo_board(DOSING_PUMP_ADDRESS_1, "PMP1");
Ezo_board dosingPump_Tank02 = Ezo_board(DOSING_PUMP_ADDRESS_2, "PMP2");

// Create an instance of the sensors
Ezo_board pHSensor_Tank01 = Ezo_board(PH_SENSOR_ADDRESS_1, "pH1");
Ezo_board pHSensor_Tank02 = Ezo_board(PH_SENSOR_ADDRESS_2, "pH2");

// pH Sensor Reading Functions
float phSensorReadTank01() {
    pHSensor_Tank01.send_read_cmd();
    // Wait for the sensor to process the command.
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    // Assume get_reading() returns a String with the sensor's response.
    String reading = pHSensor_Tank01.get_reading();
    return reading.toFloat();
}

float phSensorReadTank02() {
    pHSensor_Tank02.send_read_cmd();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    String reading = pHSensor_Tank02.get_reading();
    return reading.toFloat();
}

// Dosing Functions
void doseAcidTank01() {
    String command = "d,10"; // Change dosing volume as needed, units is in mL
    dosingPumpTank01.send_cmd(command);
    Serial.println("Acid dosing");
}

void doseAcidTank02() {
    String command = "d,10";  // Change dosing volume as needed, units is in mL
    dosingPump_Tank02.send_cmd(command);
    Serial.println("Tank 2: Acid dosing command sent.");
}

// Tank 1 Control
void controlTank01(void* parameters){
    xTaskCreatePinnedToCore(controlPH01, "pH01 Control", 2048, NULL, 1, &pH01Control_handle, app_cpu)
    xTaskCreatePinnedToCore (controlLLS01, "LLS01 Control", 2048, NULL, 1, &LLS01Control_handle, app_cpu);
    
    
    while (true){
        Serial.println("Tank 1 control loop");
        /*Configure Delay between sensor readings*/
        vTaskDelay(2000/ portTICK_PERIOD_MS);
    }
}

void controlPH01(void* parameters) {
  const uint32_t dosingDelayMinutes = 5; // Dosing delay in minutes. Change as needed.
  const TickType_t dosingDelayTicks = dosingDelayMinutes * 60000 / portTICK_PERIOD_MS;
  
  // Tick Count of last dosing
  TickType_t lastDoseTick = 0;

  while (true) {  
    float pHValue = phSensorReadTank01();

  // If pH is too high, check if it's time to dose again
    if (pHValue > 7) {
      Serial.println("Tank 1 pH reading is %.2f\n", pHValue);
      if ((xTaskGetTickCount() - lastDoseTick) >= doseDelayTicks) {
      
        doseAcidTank01();       
        lastDoseTick = xTaskGetTickCount();
    }
    
    else if (pHValue < 6) {
      // pH is too low, send an alert
      Serial.println("Tank 1 pH reading is %.2f\n", pHValue);
    }
    
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}


void controlLLS01(void* parameters) {
    pinMode(LLS_LOW_PIN, INPUT);
    pinMode(LLS_HIGH_PIN, INPUT);

    while (true) {

        bool lowSensorActive01 = digitalRead(LLS_LOW_PIN);
        bool highSensorActive = digitalRead(LLS_HIGH_PIN);
        
        if (lowSensorActive01) {
            Serial.println("Water level in Tank 1 is LOW.");
        } 
        else (highSensorActive01) {
            Serial.println("Water level in Tank 1 is HIGH.");
        }
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// TANK 2

void controlTank02(void* parameters) {
    xTaskCreatePinnedToCore(controlPH02, "pH02 Control", 2048, NULL, 1, &pH02Control_handle, app_cpu);
    xTaskCreatePinnedToCore(controlLLS02, "LLS02 Control", 2048, NULL, 1, &LLS02Control_handle, app_cpu);
    
    while (true) {
        Serial.println("Tank 2 control loop");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}


void controlPH02(void* parameters) {
    const uint32_t dosingDelayMinutes = 5; // Dosing delay in minutes
    const TickType_t dosingDelayTicks = pdMS_TO_TICKS(dosingDelayMinutes * 60000UL);
    TickType_t lastDoseTick = 0;
    
    while (true) {
        float pHValue = phSensorReadTank02();  
        
        if (pHValue > 7) {
            Serial.println("Tank 2 pH reading is %.2f", pHValue);
            if ((xTaskGetTickCount() - lastDoseTick) >= dosingDelayTicks) {
                doseAcidTank02();
                lastDoseTick = xTaskGetTickCount();
            }
        }
        else if (pHValue < 6) {
            Serial.println("Tank 2 pH reading is %.2f", pHValue);
        }
        
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void controlLLS02(void* parameters) {
    pinMode(LLS_LOW_PIN_02, INPUT);
    pinMode(LLS_HIGH_PIN_02, INPUT);
    
    while (true) {
        bool lowSensorActive02 = digitalRead(LLS_LOW_PIN_02);
        bool highSensorActive02 = digitalRead(LLS_HIGH_PIN_02);
        
        if (lowSensorActive02) {
            Serial.println("Water level in Tank 2 is LOW.");
        }
        else (highSensorActive02) {
            ("Water level in Tank 2 is HIGH.");
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
