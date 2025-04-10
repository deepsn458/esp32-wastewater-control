
void sendSensorReading(const String &sensorType,){
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
