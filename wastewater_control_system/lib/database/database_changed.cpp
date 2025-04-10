/*
WOULD NEED TO ADD THIS INTO SENSOR PUSHES:
#include <map>
std::map<String, float> pHReadings;
sensorReadings[pHSensorExample.get_name()] = rand();  
sensorReadings[pHSensorExample.get_name()] = rand();                
sensorReadings[pHSensorExample.get_name()] = rand();                  
sensorReadings[pHSensorExample.get_name()] = rand(); 
pushMultipleSensorReadings("pH", pHReadings)
*/

void pushMultipleSensorReading(const String &sensorType, const std::map<String){
    if (!app.ready()) {
        Serial.println("[Firebase] Not ready yet for pushSensorReading.");
        return;
    }
    //if the task queue is full, we wait till all the tasks have been cleared
    else if (async_client2.taskCount() == maximumTaskCount){
        clearedTasks = false;
        while (async_client2.taskCount() > 9){
            delay(50);
        }
        clearedTasks = true;
    }
    else if (clearedTasks){
        
    }
    return;
}
