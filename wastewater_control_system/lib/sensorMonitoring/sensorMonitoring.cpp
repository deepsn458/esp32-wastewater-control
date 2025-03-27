#include <Arduino.h>
#include "tankControl.h"
#include "database.h"
#include "Ezo_i2c.h"   // Atlas EZO I2C library for controlling sensors
#include "Wire.h" // Arduinos I2C Library
#include <stdlib.h>

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

const int PH_SENSOR_ADDRESS_T06 = 100;
const int PH_SENSOR_ADDRESS_T07 = 102;
const int PH_SENSOR_ADDRESS_EC1 = 103;//not yet set
const int PH_SENSOR_ADDRESS_EC2 = 104;//not yet set
const int COND_SENSOR_ADDRESS_EC1 = 105;
const int COND_SENSOR_ADDRESS_EC2 = 106;
const int COND_SENSOR_ADDRESS_T7 = 110;
const int COND_SENSOR_ADDRESS_T2 = 111;
const int DO_SENSOR_ADDRESS_T01 = 107;
const int DO_SENSOR_ADDRESS_T07 = 108;
const int TEMP_SENSOR_ADDRESS_T01 = 109;
const int SENSOR_COUNT = 11;

// pH Sensors
Ezo_board pHSensor_Tank06 = Ezo_board(PH_SENSOR_ADDRESS_T06, "PH_T06");
Ezo_board pHSensor_Tank07 = Ezo_board(PH_SENSOR_ADDRESS_T07, "PH_T07");
Ezo_board pHSensor_EC1    = Ezo_board(PH_SENSOR_ADDRESS_EC1,   "PH_EC1");
Ezo_board pHSensor_EC2    = Ezo_board(PH_SENSOR_ADDRESS_EC2,   "PH_EC2");

// Conductivity Sensors
Ezo_board condSensor_EC1  = Ezo_board(COND_SENSOR_ADDRESS_EC1, "COND_EC1");
Ezo_board condSensor_EC2  = Ezo_board(COND_SENSOR_ADDRESS_EC2, "COND_EC2");
Ezo_board condSensor_Tank7 = Ezo_board(COND_SENSOR_ADDRESS_T7, "COND_T7");
Ezo_board condSensor_Tank2 = Ezo_board(COND_SENSOR_ADDRESS_T2, "COND_T2");

// Dissolved Oxygen Sensors
Ezo_board doSensor_Tank01 = Ezo_board(DO_SENSOR_ADDRESS_T01, "DO_T01");
Ezo_board doSensor_Tank07 = Ezo_board(DO_SENSOR_ADDRESS_T07, "DO_T07");

// Create instance for temperature sensor on Tank01.
Ezo_board tempSensor_Tank01 = Ezo_board(TEMP_SENSOR_ADDRESS_T01, "TEMP_T01");
void monitorSensors(void* parameters) {
    // Set sensor read delay (in ticks). Currently set as 5000ms
    Wire.begin();
    srand(11);
    const TickType_t sensorDelay = pdMS_TO_TICKS(5000UL);

    while (true) {
        // Issue read commands to all EZO sensors

        // pH sensors:
        pHSensor_Tank06.send_read_cmd();
       // pHSensor_Tank07.send_read_cmd();
        //pHSensor_EC1.send_read_cmd();
        //pHSensor_EC2.send_read_cmd();

        // Conductivity sensors:
       // condSensor_EC1.send_read_cmd();
        condSensor_EC2.send_read_cmd();
       // condSensor_Tank7.send_read_cmd();
        //condSensor_Tank2.send_read_cmd();

        // Dissolved Oxygen sensors:
        //doSensor_Tank01.send_read_cmd();
        doSensor_Tank07.send_read_cmd();

        // Temperature sensor:
        tempSensor_Tank01.send_read_cmd();

        // Wait for sensors to process the command (might need to adjust)
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        pHSensor_Tank06.receive_read_cmd();
       // pHSensor_Tank07.receive_read_cmd();
        //pHSensor_EC1.receive_read_cmd();
        //pHSensor_EC2.receive_read_cmd();

        // Conductivity sensors:
        //condSensor_EC1.receive_read_cmd();
        condSensor_EC2.receive_read_cmd();
        //condSensor_Tank7.receive_read_cmd();
        //condSensor_Tank2.receive_read_cmd();

        // Dissolved Oxygen sensors:
        //doSensor_Tank01.receive_read_cmd();
        doSensor_Tank07.receive_read_cmd();

        // Temperature sensor:
        tempSensor_Tank01.receive_read_cmd();
        
        // Retrieve sensor readings
        float ph_t06    = pHSensor_Tank06.get_last_received_reading();
        //float ph_t07    = pHSensor_Tank07.get_last_received_reading();
        //float ph_ec1    = pHSensor_EC1.get_last_received_reading();
        //float ph_ec2    = pHSensor_EC2.get_last_received_reading();

       // float cond_ec1  = condSensor_EC1.get_last_received_reading();
        float cond_ec2  = condSensor_EC2.get_last_received_reading();
        //float cond_t7   = condSensor_Tank7.get_last_received_reading();
       // float cond_t2   = condSensor_Tank2.get_last_received_reading();

       // float do_t01    = doSensor_Tank01.get_last_received_reading();
        float do_t07    = doSensor_Tank07.get_last_received_reading();

        float temp_t01  = tempSensor_Tank01.get_last_received_reading();


        // Read digital LLS sensors
        // ADD IN HERE FOR TANK 6, 7, 8

        // Data as CSV string for LABVIEW to parse
        /*
        char ph_t06_arr[5];char ph_t07_arr[5];char ph_ec1_arr[5];char ph_ec2_arr[5];
        char cond_ec1_arr[5];char cond_t7_arr[5];char cond_t2_arr[5];char do_t01_arr[5];
        char do_t07_arr[5]; char temp_t01_arr[5]; char cond_ec2_arr[5];
        sprintf(ph_t06_arr,"%.2f", ph_t06); sprintf(ph_t07_arr,"%.2f", ph_t07);
        sprintf(ph_ec1_arr,"%.2f", ph_ec1);sprintf(ph_ec2_arr,"%.2f", ph_ec2);
        sprintf(cond_ec1_arr,"%.2f", cond_ec1);sprintf(cond_t2_arr,"%.2f", cond_t2);
        sprintf(cond_t7_arr,"%.2f", cond_t7);  sprintf(do_t01_arr,"%.2f", do_t01);
        sprintf(do_t07_arr,"%.2f", do_t07); sprintf(temp_t01_arr,"%.2f", temp_t01);
         sprintf(cond_ec2_arr,"%.2f", cond_ec2);
         */
        /*
        Serial.print("pH_T06,"); Serial.print(ph_t06_arr); Serial.print(",");
        Serial.print("pH_T07,"); Serial.print(ph_t07_arr); Serial.print(",");
        Serial.print("pH_EC1,"); Serial.print(ph_ec1_arr); Serial.print(",");
        Serial.print("pH_EC2,"); Serial.print(ph_ec2); Serial.print(",");

        Serial.print("Cond_EC1,"); Serial.print(cond_ec1_arr); Serial.print(",");
        Serial.print("Cond_EC2,"); Serial.print(cond_ec2_arr); Serial.print(",");
        Serial.print("Cond_T7,"); Serial.print(cond_t7_arr); Serial.print(",");
        Serial.print("Cond_T2,"); Serial.print(cond_t2_arr); Serial.print(",");

        Serial.print("DO_T01,"); Serial.print(do_t01_arr); Serial.print(",");
        Serial.print("DO_T07,"); Serial.print(do_t07_arr); Serial.print(",");

        Serial.print("Temp_T01,"); Serial.print(temp_t01_arr); Serial.print(",");
        */
        //sends all the sensor readings to the database
        pushSensorReading("ph", pHSensor_Tank06.get_name(), ph_t06+rand());
        //pushSensorReading("ph", pHSensor_Tank07.get_name(), ph_t07);
        //pushSensorReading("ph", pHSensor_EC1.get_name(), ph_ec1);
        //pushSensorReading("ph", pHSensor_EC2.get_name(), ph_ec2);
        //pushSensorReading("conductivity", condSensor_EC1.get_name(), cond_ec1);
        pushSensorReading("conductivity",condSensor_EC2.get_name(), cond_ec2+rand());
       // pushSensorReading("conductivity",condSensor_Tank7.get_name(), cond_t7);
        //pushSensorReading("conductivity",condSensor_Tank2.get_name(), cond_t2);
        //pushSensorReading("dissolved02",doSensor_Tank01.get_name(), do_t01);
        pushSensorReading("dissolved02",doSensor_Tank07.get_name(), do_t07+rand());
        pushSensorReading("temperature",tempSensor_Tank01.get_name(), temp_t01+rand());
        vTaskDelay(sensorDelay);
    }
}
