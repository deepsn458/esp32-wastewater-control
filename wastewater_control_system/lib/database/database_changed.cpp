/*
WOULD NEED TO ADD THIS INTO SENSOR PUSHES:
#include <map>
std::map<String, float> pHReadings;
sensorReadings[pHSensorExample.get_name()] = rand();  
sensorReadings[pHSensorExample.get_name()] = rand();                
sensorReadings[pHSensorExample.get_name()] = rand();                  
sensorReadings[pHSensorExample.get_name()] = rand(); 
pushpHReadings(pHReadings)
*/

void pushMultipleSensorReading(const std::map<String){
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
        object_t tsObj;
        object_t fullTsObj;
        object_t sensor1Obj;
        object_t sensor2Obj;
        object_t fullMsgObj;
        
        String path = "/devices/" + "esp32" + "/batchPushTest/";

        JsonWriter writer;

        writer.create(tsObj, ".sv", "timestamp");
        writer.create(fullTsObj, "timestamp", tsObj);
        
        writer.create(sensor1Obj, "PH01", number_t(readingValue1, 2));
        writer.create(sensor2Obj, "PH02", number_t(readingValue2, 2));
        
        // Join the sensor objects with the timestamp. The count (3) indicates we are merging three objects.
        writer.join(fullMsgObj, 3, sensor1Obj, sensor2Obj, fullTsObj);
        
        Serial.println(fullMsgObj);
        
        Database.push<object_t>(async_client2, path, fullMsgObj, processData, "");
        writer.join(fullMsgObj, 3, sensor1Obj, sensor2Obj, fullTsObj);
        Serial.println(fullMsgObj);
        Database.push<object_t>(async_client2, path, fullMsgObj, processData, "");
    }
    return;
}
