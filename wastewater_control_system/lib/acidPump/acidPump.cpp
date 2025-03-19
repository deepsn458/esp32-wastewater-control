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
Ezo_board acidPump1 = Ezo_board(56, "acidPump1");
Ezo_board acidPump2 = Ezo_board(57, "acidPump2");
Ezo_board acidPump3 = Ezo_board(58, "acidPump3");

void doseAcid(void* params){
    Serial.begin(115200);
    Wire.begin();

    for(;;){
        acidPump1.send_cmd_with_num("d,",10);
        vTaskDelay(100/portTICK_PERIOD_MS);
        acidPump1.send_cmd("x");
        acidPump2.send_cmd_with_num("d,",10);
        vTaskDelay(100/portTICK_PERIOD_MS);
        acidPump2.send_cmd("x");
        acidPump3.send_cmd_with_num("d,",10);
        vTaskDelay(100/portTICK_PERIOD_MS);
        acidPump3.send_cmd("x");
        Serial.println("uea");
        vTaskDelay(3000/portTICK_PERIOD_MS);

    }
}