#include <Arduino.h>
#include <Wire.h>
#include <Ezo_i2c.h>
#include <sensorCal.h>

Ezo_board dOProbe = Ezo_board(108, "q");
char received[32];
int same;
float last_reading;
void calibrateDoProbe(void* params){
    Wire.begin();
    Serial.begin(115200);
    //delay 10 seconds
    vTaskDelay(10000/portTICK_PERIOD_MS);
    
    for (;;){
        dOProbe.send_read_cmd();
        vTaskDelay(200/portTICK_PERIOD_MS);
        dOProbe.receive_read_cmd();
        //checks if the prvious value is the same as the current value
        if (last_reading == dOProbe.get_last_received_reading()){
            same++;
        }
        else{
            same = 0;
        }
        Serial.println(dOProbe.get_last_received_reading());
        last_reading = dOProbe.get_last_received_reading();
        //if the value has stabilized, send the calibration command
        if (same == 6){
            dOProbe.send_cmd("Cal");
            vTaskDelay(1300/portTICK_PERIOD_MS);
            dOProbe.receive_cmd(received,32);
            Serial.println(received);
            break;
        }
        vTaskDelay(500/portTICK_PERIOD_MS);
    }
}