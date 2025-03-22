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
// Replace these with actual Atlas Scientific addresses
#define DOSING_PUMP_ADDRESS_1 0x38  // Example address for Tank 1 dosing pump
#define DOSING_PUMP_ADDRESS_2 0x39  // Example address for Tank 2 dosing pump
#define PH_SENSOR_ADDRESS_1   0x63  // Example address for Tank 1 pH sensor
#define PH_SENSOR_ADDRESS_2   0x65  // Example address for Tank 2 pH sensor

// Liquid Level Sensor (LLS) Pins
#define LLS_LOW_PIN       2  // Tank 1 low level sensor pin
#define LLS_HIGH_PIN      3  // Tank 1 high level sensor pin
#define LLS_LOW_PIN_02    4  // Tank 2 low level sensor pin
#define LLS_HIGH_PIN_02   5  // Tank 2 high level sensor pin

//timer handle for tank 2
TimerHandle_t controlTank2Timer;

// Task handles for Tank 2
TaskHandle_t pH02Control_handle = NULL;
TaskHandle_t LLS02Control_handle = NULL;

// Create an instance of the dosing pumps
Ezo_board dosingPumpTank01 = Ezo_board(DOSING_PUMP_ADDRESS_1, "PMP1");
Ezo_board dosingPumpTank02 = Ezo_board(DOSING_PUMP_ADDRESS_2, "PMP2");

// Create an instance of the sensors
Ezo_board pHSensor_Tank01 = Ezo_board(PH_SENSOR_ADDRESS_1, "pH1");
Ezo_board pHSensor_Tank02 = Ezo_board(101, "pH2");

//callback function when the timer expires
void startControlTank2(TimerHandle_t controlTank2Timer){
    Serial.println(pHSensor_Tank02.get_address());
    xTaskCreatePinnedToCore(controlPH02, "pH02 Control", 1024, NULL, 3, &pH02Control_handle, app_cpu);
    xTaskCreatePinnedToCore(controlLLS02, "LLS02 Control", 1024, NULL, 1, &LLS02Control_handle, app_cpu);
}
// pH Sensor Reading Functions
float phSensorReadTank01() {
    pHSensor_Tank01.send_read_cmd();
    // Wait for the sensor to process the command.
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    // Assume get_reading() returns a String with the sensor's response.
    pHSensor_Tank01.receive_read_cmd();
    return(pHSensor_Tank01.get_last_received_reading());
}

float phSensorReadTank02() {
    pHSensor_Tank02.send_read_cmd();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    pHSensor_Tank02.receive_read_cmd();
    return(pHSensor_Tank02.get_last_received_reading());
}

// Dosing Functions
void doseAcidTank01() {
    
    // Change dosing volume as needed, units is in mL
    dosingPumpTank01.send_cmd_with_num("d,", 10);
    Serial.println("Acid dosing");
}

void doseAcidTank02() {
    // Change dosing volume as needed, units is in mL
    dosingPumpTank02.send_cmd_with_num("d,", 10);
    Serial.println("Tank 2: Acid dosing command sent.");
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
      Serial.print("Tank 1 pH reading is"); Serial.println(pHValue);
      if ((xTaskGetTickCount() - lastDoseTick) >= dosingDelayTicks) {
      
        doseAcidTank01();       
        lastDoseTick = xTaskGetTickCount();
    }
    
    else if (pHValue < 6) {
      // pH is too low, send an alert
      Serial.print("Tank 1 pH reading is");Serial.println(pHValue);
    }
    
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
 }
}


void controlLLS01(void* parameters) {
    pinMode(LLS_LOW_PIN, INPUT);
    pinMode(LLS_HIGH_PIN, INPUT);

    while (true) {

        bool lowSensorActive01 = digitalRead(LLS_LOW_PIN);
        bool highSensorActive01 = digitalRead(LLS_HIGH_PIN);
        
        if (lowSensorActive01) {
            Serial.println("Water level in Tank 1 is LOW.");
        } 
        else if(highSensorActive01) {
            Serial.println("Water level in Tank 1 is HIGH.");
        }
        
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

// TANK 2
void controlPH02(void* parameters) {
    const uint32_t dosingDelayMinutes = 1; // Dosing delay in minutes
    const TickType_t dosingDelayTicks = pdMS_TO_TICKS(dosingDelayMinutes * 30000UL);
    TickType_t lastDoseTick = 0;
    
    while (true) {
        float pHValue = phSensorReadTank02();  
        
        if (pHValue > 7) {
            Serial.print("Tank 2 pH reading is"); Serial.println(pHValue);
            if ((xTaskGetTickCount() - lastDoseTick) >= dosingDelayTicks) {
                doseAcidTank02();
                lastDoseTick = xTaskGetTickCount();
            }
        }
        else if (pHValue < 6) {
            Serial.print("Tank 2 pH reading is"); Serial.println(pHValue);
        }
        
        vTaskDelay(5000 / portTICK_PERIOD_MS);
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
        else if (highSensorActive02) {
        Serial.println("Water level in Tank 2 is HIGH.");
        
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}
}
