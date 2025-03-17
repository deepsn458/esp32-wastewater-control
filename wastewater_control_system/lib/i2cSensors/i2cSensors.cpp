#include <Arduino.h>
#include <Ezo_i2c.h>
#include <Wire.h>

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

const int processCmdDelay = 1000;
const TickType_t sensorDelay = pdMS_TO_TICKS(5000UL);
//Ezo_board pHSensor = Ezo_board(99, "PH01");
//Ezo_board condSensor = Ezo_board(97, "cond01");
Ezo_board doSensor = Ezo_board(97, "do01");
Ezo_board tempSensor = Ezo_board(102, "temp01");
Ezo_board pGSensor = Ezo_board(106, "pressure 01");


void sensorMonitoringTask(void* parameters){
    Serial.begin(115200);
    Wire.begin();

    for (;;){
        doSensor.send_read_cmd();
        tempSensor.send_read_cmd();
        pGSensor.send_read_cmd();
        vTaskDelay(processCmdDelay / portTICK_PERIOD_MS);
        tempSensor.receive_read_cmd();
        pGSensor.receive_read_cmd();
        doSensor.receive_read_cmd();

    
        float doReading = doSensor.get_last_received_reading();
        float tempReading = tempSensor.get_last_received_reading();
        float pGReading = pGSensor.get_last_received_reading();

        //prints out the received reading
        Serial.print("Pressure Reading: "); Serial.println(pGReading);
        Serial.print("do Reading: "); Serial.println(doReading);
        Serial.print("temp reading: "); Serial.println(tempReading);
        vTaskDelay(sensorDelay);
    }
    
}


