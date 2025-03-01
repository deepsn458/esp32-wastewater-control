#include <Arduino.h>
#include "tankControl.h"
#include "Ezo_i2c.h"   // Atlas EZO I2C library for controlling sensors
#include "Wire.h" // Arduinos I2C Library

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Create instances of turbidity sensors using the EZO library
Ezo_board turbiditySensor_Tank01 = Ezo_board(TURBIDITY_SENSOR_ADDRESS_1, "Turb1");
Ezo_board turbiditySensor_EC01   = Ezo_board(TURBIDITY_SENSOR_ADDRESS_2, "TurbEC1");
Ezo_board turbiditySensor_EC02   = Ezo_board(TURBIDITY_SENSOR_ADDRESS_3, "TurbEC2");

void sensorMonitoringTask(void* parameters) {
    // Set sensor read delay (in ticks). Currently set as 5000ms 
    const TickType_t sensorDelay = pdMS_TO_TICKS(5000UL);

    while (true) {
        turbiditySensor_Tank01.send_read_cmd();
        turbiditySensor_EC01.send_read_cmd();
        turbiditySensor_EC02.send_read_cmd();

        // Wait for sensors to process the command (adjust if necessary)
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // Retrieve sensor reading
        String turbTank01 = turbiditySensor_Tank01.get_reading();
        String turbEC01   = turbiditySensor_EC01.get_reading();
        String turbEC02   = turbiditySensor_EC02.get_reading();

        // Data as CSV string for LABVIEW to parse.
        Serial.print("Turbidity,");
        Serial.print("Tank01,");
        Serial.print(turbTank01);
        Serial.print(",EC01,");
        Serial.print(turbEC01);
        Serial.print(",EC02,");
        Serial.println(turbEC02);

        vTaskDelay(sensorDelay);
    }
}
