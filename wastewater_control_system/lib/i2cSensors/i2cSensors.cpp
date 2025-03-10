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
Ezo_board pHSensor = Ezo_board(99, "PH01");
Ezo_board condSensor = Ezo_board(97, "cond01");
Ezo_board doSensor = Ezo_board(100, "do01");
Ezo_board tempSensor = Ezo_board(102, "temp01");


void sensorMonitoringTask(void* parameters){
    Serial.begin(115200);
    Wire.begin();

    for (;;){
        pHSensor.send_read_cmd();
        condSensor.send_read_cmd();

        vTaskDelay(processCmdDelay / portTICK_PERIOD_MS);

        float phReading = pHSensor.get_last_received_reading();
        float condReading = condSensor.get_last_received_reading();
        float doReading = doSensor.get_last_received_reading();
        float tempReading = tempSensor.get_last_received_reading();

        //prints out the received reading
        Serial.print("PH Reading: "); Serial.println(phReading);
        Serial.print("cond reading: "); Serial.println(condReading);
        Serial.print("do Reading: "); Serial.println(doReading);
        Serial.print("temp reading: "); Serial.println(tempReading);
        vTaskDelay(sensorDelay);
    }
    
}


