void pushMultipleSensorReading(const String& sensor1Id, float sensor1Value,
                              const String& sensor2Id, float sensor2Value,
                              const String& sensor3Id, float sensor3Value,
                              const String& sensor4Id, float sensor4Value){
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
        object_t sensorObj1;
        object_t sensorObj2;
        object_t sensorObj3;
        object_t sensorObj4;
        object_t fullMsgObj;
        
        String path = "/devices/" + "esp32" + "/batchPushTest/";

        JsonWriter writer;

        writer.create(tsObj, ".sv", "timestamp");
        writer.create(fullTsObj, "timestamp", tsObj);

        writer.create(sensorObj1, sensor1Id, number_t(sensor1Value, 2));
        writer.create(sensorObj2, sensor2Id, number_t(sensor2Value, 2));
        writer.create(sensorObj3, sensor3Id, number_t(sensor3Value, 2));
        writer.create(sensorObj4, sensor4Id, number_t(sensor4Value, 2));
        
        writer.join(fullMsgObj, 5, sensorObj1, sensorObj2, sensorObj3, sensorObj4, fullTsObj);
        
        Serial.println(fullMsgObj);
        
        Database.push<object_t>(async_client2, path, fullMsgObj, processData, "pushSensorTask");
    }
    return;
}
