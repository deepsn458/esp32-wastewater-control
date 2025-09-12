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

const int PH_SENSOR_ADDRESS_TREATED = 103;
const int PH_SENSOR_ADDRESS_EC1 = 101;
const int PH_SENSOR_ADDRESS_EC2 = 100;
const int PH_SENSOR_ADDRESS_HYDROLYSIS = 99;
const int PH_SENSOR_ADDRESS_ADJUSTMENT = 102;

const int COND_SENSOR_ADDRESS_EC1 = 112;
const int COND_SENSOR_ADDRESS_EC2 = 105;
const int COND_SENSOR_ADDRESS_TREATED = 106;
const int COND_SENSOR_ADDRESS_ADJUSTMENT = 113;

const int DO_SENSOR_ADDRESS_HYDROLYSIS = 108;
const int DO_SENSOR_ADDRESS_TREATED = 97;
const int TEMP_SENSOR_ADDRESS_HYDROLYSIS = 109;
//const int SENSOR_COUNT = 11;

// pH Sensors
//Ezo_board pHSensor_Tank06 = Ezo_board(PH_SENSOR_ADDRESS_T06, "PH_T06");
Ezo_board pHSensor_treated = Ezo_board(PH_SENSOR_ADDRESS_TREATED, "PH_TR");
Ezo_board pHSensor_EC1    = Ezo_board(PH_SENSOR_ADDRESS_EC1,   "PH_ED1");
Ezo_board pHSensor_EC2    = Ezo_board(PH_SENSOR_ADDRESS_EC2,   "PH_ED2");
Ezo_board pHSensor_hydrolysis = Ezo_board(PH_SENSOR_ADDRESS_HYDROLYSIS, "PH_HYD");

// Conductivity Sensors
Ezo_board condSensor_EC1  = Ezo_board(COND_SENSOR_ADDRESS_EC1, "COND_ED1");
Ezo_board condSensor_EC2  = Ezo_board(COND_SENSOR_ADDRESS_EC2, "COND_ED2");
Ezo_board condSensor_treated = Ezo_board(COND_SENSOR_ADDRESS_TREATED, "COND_TR");
Ezo_board condSensor_adjustment = Ezo_board(COND_SENSOR_ADDRESS_ADJUSTMENT, "COND_ADJ");

// Dissolved Oxygen Sensors
Ezo_board doSensor_hydrolysis = Ezo_board(DO_SENSOR_ADDRESS_HYDROLYSIS, "DO_HYD");
Ezo_board doSensor_treated = Ezo_board(DO_SENSOR_ADDRESS_TREATED, "DO_TR");

// Create instance for temperature sensor on Tank01.
Ezo_board tempSensor_hydrolysis = Ezo_board(TEMP_SENSOR_ADDRESS_HYDROLYSIS, "TEMP_HYD");
void monitorSensors(void* parameters) {
    // Set sensor read delay (in ticks). Currently set as 5000ms
    //Wire.begin();
    srand(11);
    const TickType_t sensorDelay = pdMS_TO_TICKS(5000UL);

    while (true) {
        // Issue read commands to all EZO sensors

        // pH sensors:
        pHSensor_treated.send_read_cmd();
        pHSensor_EC1.send_read_cmd();
        pHSensor_EC2.send_read_cmd();
        pHSensor_hydrolysis.send_read_cmd();

        // Conductivity sensors:
        condSensor_EC1.send_read_cmd();
        condSensor_EC2.send_read_cmd();
        condSensor_treated.send_read_cmd();
        condSensor_adjustment.send_read_cmd();

        // Dissolved Oxygen sensors:
        doSensor_hydrolysis.send_read_cmd();
        
        doSensor_treated.send_read_cmd();

        // Temperature sensor:
        tempSensor_hydrolysis.send_read_cmd();

        // Wait for sensors to process the command (might need to adjust)
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        
        //pHSensor_Tank06.receive_read_cmd();
        pHSensor_treated.receive_read_cmd();
        pHSensor_EC1.receive_read_cmd();
        pHSensor_EC2.receive_read_cmd();
        pHSensor_hydrolysis.receive_read_cmd();

        // Conductivity sensors:
        condSensor_EC1.receive_read_cmd();
        condSensor_EC2.receive_read_cmd();
        condSensor_treated.receive_read_cmd();
        condSensor_adjustment.receive_read_cmd();

        // Dissolved Oxygen sensors:
        doSensor_hydrolysis.receive_read_cmd();
        doSensor_treated.receive_read_cmd();

        // Temperature sensor:
        tempSensor_hydrolysis.receive_read_cmd();
        
        // Retrieve sensor readings
        //float ph_t06    = pHSensor_Tank06.get_last_received_reading();
        float ph_tr    = pHSensor_treated.get_last_received_reading();
        float ph_ec1    = pHSensor_EC1.get_last_received_reading();
        float ph_ec2    = pHSensor_EC2.get_last_received_reading();
        float ph_hyd = pHSensor_hydrolysis.get_last_received_reading();

        float cond_ec1  = condSensor_EC1.get_last_received_reading();
        float cond_ec2  = condSensor_EC2.get_last_received_reading();
        float cond_tr   = condSensor_treated.get_last_received_reading();
        float cond_adj   = condSensor_adjustment.get_last_received_reading();

       float do_hyd    = doSensor_hydrolysis.get_last_received_reading();
        float do_tr    = doSensor_treated.get_last_received_reading();

        float temp_t01  = tempSensor_hydrolysis.get_last_received_reading();


        // Read digital LLS sensors
        // ADD IN HERE FOR TANK 6, 7, 8

        //sends all the sensor readings to the database
        //pushSensorReading("ph", pHSensor_Tank06.get_name(), rand());
        pushSensorReading("ph", pHSensor_hydrolysis.get_name(),ph_hyd);
        pushSensorReading("ph", pHSensor_treated.get_name(), ph_tr);
        pushSensorReading("ph", pHSensor_EC1.get_name(), ph_ec1);
        pushSensorReading("ph", pHSensor_EC2.get_name(), ph_ec2);
        pushSensorReading("ph", pHSensor_hydrolysis.get_name(),ph_hyd);
        pushSensorReading("conductivity", condSensor_EC1.get_name(), cond_ec1);
        pushSensorReading("conductivity",condSensor_EC2.get_name(), cond_ec2);
        pushSensorReading("conductivity",condSensor_treated.get_name(), cond_tr);
        pushSensorReading("conductivity",condSensor_adjustment.get_name(), cond_adj);
        pushSensorReading("dissolved02",doSensor_hydrolysis.get_name(), do_hyd);
        pushSensorReading("dissolved02",doSensor_treated.get_name(), do_tr);
        pushSensorReading("temperature",tempSensor_hydrolysis.get_name(), temp_t01);
        vTaskDelay(sensorDelay);
    }
}
