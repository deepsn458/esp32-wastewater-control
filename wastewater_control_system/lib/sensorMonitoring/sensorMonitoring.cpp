#include <Arduino.h>
#include "tankControl.h"
#include "Ezo_i2c.h"   // Atlas EZO I2C library for controlling sensors
#include "Wire.h" // Arduinos I2C Library

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// pH Sensors
Ezo_board pHSensor_Tank06 = Ezo_board(PH_SENSOR_ADDRESS_T06, "pH_T06");
Ezo_board pHSensor_Tank07 = Ezo_board(PH_SENSOR_ADDRESS_T07, "pH_T07");
Ezo_board pHSensor_EC1    = Ezo_board(PH_SENSOR_ADDRESS_EC1,   "pH_EC1");
Ezo_board pHSensor_EC2    = Ezo_board(PH_SENSOR_ADDRESS_EC2,   "pH_EC2");

// Conductivity Sensors
Ezo_board condSensor_EC1  = Ezo_board(COND_SENSOR_ADDRESS_EC1, "Cond_EC1");
Ezo_board condSensor_EC2  = Ezo_board(COND_SENSOR_ADDRESS_EC2, "Cond_EC2");
Ezo_board condSensor_Tank7 = Ezo_board(COND_SENSOR_ADDRESS_T7, "Cond_T7");
Ezo_board condSensor_Tank2 = Ezo_board(COND_SENSOR_ADDRESS_T2, "Cond_T2");

// Dissolved Oxygen Sensors
Ezo_board doSensor_Tank01 = Ezo_board(DO_SENSOR_ADDRESS_T01, "DO_T01");
Ezo_board doSensor_Tank07 = Ezo_board(DO_SENSOR_ADDRESS_T07, "DO_T07");

// Create instance for temperature sensor on Tank01.
Ezo_board tempSensor_Tank01 = Ezo_board(TEMP_SENSOR_ADDRESS_T01, "Temp_T01");


// Turbidity Sensors
Ezo_board turbiditySensor_Tank01 = Ezo_board(TURBIDITY_SENSOR_ADDRESS_1, "Turb1");
Ezo_board turbiditySensor_EC1   = Ezo_board(TURBIDITY_SENSOR_ADDRESS_2, "TurbEC1");
Ezo_board turbiditySensor_EC2   = Ezo_board(TURBIDITY_SENSOR_ADDRESS_3, "TurbEC2");

void sensorMonitoringTask(void* parameters) {
    // Set sensor read delay (in ticks). Currently set as 5000ms 
    const TickType_t sensorDelay = pdMS_TO_TICKS(5000UL);

    while (true) {
        // Issue read commands to all EZO sensors

        // pH sensors:
        pHSensor_Tank06.send_read_cmd();
        pHSensor_Tank07.send_read_cmd();
        pHSensor_EC1.send_read_cmd();
        pHSensor_EC2.send_read_cmd();

        // Conductivity sensors:
        condSensor_EC1.send_read_cmd();
        condSensor_EC2.send_read_cmd();
        condSensor_Tank7.send_read_cmd();
        condSensor_Tank2.send_read_cmd();

        // Dissolved Oxygen sensors:
        doSensor_Tank01.send_read_cmd();
        doSensor_Tank07.send_read_cmd();

        // Temperature sensor:
        tempSensor_Tank01.send_read_cmd();
        
        // Turbidity sensors:
        turbiditySensor_Tank01.send_read_cmd();
        turbiditySensor_EC1.send_read_cmd();
        turbiditySensor_EC2.send_read_cmd();

        // Wait for sensors to process the command (might need to adjust)
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // Retrieve sensor readings
        String ph_t06    = pHSensor_Tank06.get_lreading();
        String ph_t07    = pHSensor_Tank07.get_reading();
        String ph_ec1    = pHSensor_EC1.get_reading();
        String ph_ec2    = pHSensor_EC2.get_reading();

        String cond_ec1  = condSensor_EC1.get_reading();
        String cond_ec2  = condSensor_EC2.get_reading();
        String cond_t7   = condSensor_Tank7.get_reading();
        String cond_t2   = condSensor_Tank2.get_reading();

        String do_t01    = doSensor_Tank01.get_reading();
        String do_t07    = doSensor_Tank07.get_reading();

        String temp_t01  = tempSensor_Tank01.get_reading();

        String turb_t01 = turbiditySensor_Tank01.get_reading();
        String turbEC1   = turbiditySensor_EC1.get_reading();
        String turbEC2   = turbiditySensor_EC2.get_reading();

        // Read digital LLS sensors
        // ADD IN HERE FOR TANK 6, 7, 8

        // Data as CSV string for LABVIEW to parse
        Serial.print("pH_T06,"); Serial.print(ph_t06); Serial.print(",");
        Serial.print("pH_T07,"); Serial.print(ph_t07); Serial.print(",");
        Serial.print("pH_EC1,"); Serial.print(ph_ec1); Serial.print(",");
        Serial.print("pH_EC2,"); Serial.print(ph_ec2); Serial.print(",");

        Serial.print("Cond_EC1,"); Serial.print(cond_ec1); Serial.print(",");
        Serial.print("Cond_EC2,"); Serial.print(cond_ec2); Serial.print(",");
        Serial.print("Cond_T7,"); Serial.print(cond_t7); Serial.print(",");
        Serial.print("Cond_T2,"); Serial.print(cond_t2); Serial.print(",");

        Serial.print("DO_T01,"); Serial.print(do_t01); Serial.print(",");
        Serial.print("DO_T07,"); Serial.print(do_t07); Serial.print(",");

        Serial.print("Temp_T01,"); Serial.print(temp_t01); Serial.print(",");
        
        Serial.print("T_T01,"); Serial.print(turb_t01);
        Serial.print("T_EC1,"); Serial.print(turbEC1);
        Serial.print("T_EC2,"); Serial.println(turbEC2);

        vTaskDelay(sensorDelay);
    }
}
